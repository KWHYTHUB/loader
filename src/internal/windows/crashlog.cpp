#define WIN32_LEAN_AND_MEAN

#include <Sapphire.hpp>

#ifdef SAPPHIRE_IS_WINDOWS

#include "../crashlog.hpp"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <errhandlingapi.h>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <DbgHelp.h>

USE_SAPPHIRE_NAMESPACE();

static bool g_lastLaunchCrashed = false;

static std::string getDateString(bool filesafe) {
    const auto now = std::time(nullptr);
	const auto tm = *std::localtime(&now);
    std::ostringstream oss;
    if (filesafe) {
        oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    } else {
        oss << std::put_time(&tm, "%Y/%m/%d %H:%M:%S");
    }
    return oss.str();
}

static std::string getModuleName(HMODULE module, bool fullPath = true) {
	char buffer[MAX_PATH];
	if (!GetModuleFileNameA(module, buffer, MAX_PATH)) {
		return "<Unknown>";
    }
	if (fullPath) {
		return buffer;
    }
	return ghc::filesystem::path(buffer).filename().string();
}

static const char* getExceptionCodeString(DWORD code) {
	#define EXP_STR(c) case c: return #c
	switch (code) {
		EXP_STR(EXCEPTION_ACCESS_VIOLATION);
		EXP_STR(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
		EXP_STR(EXCEPTION_STACK_OVERFLOW);
		EXP_STR(EXCEPTION_ILLEGAL_INSTRUCTION);
		EXP_STR(EXCEPTION_IN_PAGE_ERROR);
		EXP_STR(EXCEPTION_BREAKPOINT);
		EXP_STR(EXCEPTION_DATATYPE_MISALIGNMENT);
		EXP_STR(EXCEPTION_FLT_DENORMAL_OPERAND);
		EXP_STR(EXCEPTION_FLT_DIVIDE_BY_ZERO);
		EXP_STR(EXCEPTION_FLT_INEXACT_RESULT);
		EXP_STR(EXCEPTION_FLT_INVALID_OPERATION);
		EXP_STR(EXCEPTION_FLT_OVERFLOW);
		EXP_STR(EXCEPTION_INT_DIVIDE_BY_ZERO);
		default: return "<Unknown>";
	}
	#undef EXP_STR
}

static HMODULE handleFromAddress(const void* addr) {
    HMODULE module = nullptr;
    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCTSTR)addr,
        &module
    );
    return module;
}

static Mod* modFromAddress(PVOID exceptionAddress) {
    auto modulePath = getModuleName(handleFromAddress(exceptionAddress), true);
    for (auto& mod : Loader::get()->getAllMods()) {
        if (mod->getBinaryPath() == modulePath) {
            return mod;
        }
    }
    return nullptr;
}

static void printAddr(
    std::ostream& stream, const void* addr, bool fullPath = true
) {
	HMODULE module = nullptr;

	if (GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCTSTR)addr,
        &module
    )) {
        static std::set<HMODULE> s_initialized {};

        if (!s_initialized.count(module)) {
            if (SymInitialize(
                module,
                nullptr,
                true
            )) {
                s_initialized.insert(module);
            }
        }

        // calculate base + [address]
		const auto diff = as<uintptr_t>(addr) - as<uintptr_t>(module);
		stream
            << getModuleName(module, fullPath) << " + "
            << std::hex << diff << std::dec;

        { // symbol stuff
            DWORD64 displacement;

            char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
            PSYMBOL_INFO symbolInfo = reinterpret_cast<PSYMBOL_INFO>(buffer);

            symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbolInfo->MaxNameLen = MAX_SYM_NAME;

            if (SymFromAddr(
                module,
                static_cast<DWORD64>(as<uintptr_t>(addr)),
                &displacement,
                symbolInfo
            )) {
                stream
                    << " ("
                    << std::string(symbolInfo->Name, symbolInfo->NameLen)
                    << " + " << displacement << ")";
            }
        }

	} else {
		stream << addr;
	}
}

// https://stackoverflow.com/a/50208684/9124836
static void walkStack(std::ostream& stream, PCONTEXT context) {
    STACKFRAME64 stack;
	memset(&stack, 0, sizeof(STACKFRAME64));

	auto process = GetCurrentProcess();
	auto thread = GetCurrentThread();
	stack.AddrPC.Offset = context->Eip;
	stack.AddrPC.Mode = AddrModeFlat;
	stack.AddrStack.Offset = context->Esp;
	stack.AddrStack.Mode = AddrModeFlat;
	stack.AddrFrame.Offset = context->Ebp;
	stack.AddrFrame.Mode = AddrModeFlat;

    // size_t frame = 0;
    while (true) {
		if (!StackWalk64(
			IMAGE_FILE_MACHINE_I386,
			process,
			thread,
			&stack,
			context,
			nullptr,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			nullptr
        )) break;

		stream << " - ";
        printAddr(stream, reinterpret_cast<void*>(stack.AddrPC.Offset));
        stream << std::endl;
    }
}

static void printRegisters(std::ostream& stream, PCONTEXT context) {
    stream
        << "EAX: " << context->Eax << "\n"
        << "EBX: " << context->Ebx << "\n"
        << "ECX: " << context->Ecx << "\n"
        << "EDX: " << context->Edx << "\n"
        << "EBP: " << context->Ebp << "\n"
        << "ESP: " << context->Esp << "\n"
        << "EDI: " << context->Edi << "\n"
        << "ESI: " << context->Esi << "\n"
        << "EIP: " << context->Eip << "\n";
}

static void printInfo(
    std::ostream& stream, LPEXCEPTION_POINTERS info, Mod* faultyMod
) {
    stream
	    << "Faulty Module: " << getModuleName(
            handleFromAddress(info->ExceptionRecord->ExceptionAddress),
            true
        ) << "\n"
        << "Faulty Mod: " << (faultyMod ? faultyMod->getID() : "<Unknown>")
        << "\n"
	    << "Exception Code: " << std::hex << info->ExceptionRecord->ExceptionCode
        << " (" << getExceptionCodeString(info->ExceptionRecord->ExceptionCode)
        << ")" << std::dec << "\n"
	    << "Exception Flags: "
        << info->ExceptionRecord->ExceptionFlags << "\n"
	    << "Exception Address: " << info->ExceptionRecord->ExceptionAddress
        << " (";
	printAddr(stream, info->ExceptionRecord->ExceptionAddress, false);
    stream
        << ")" << "\n"
	    << "Number Parameters: "
        << info->ExceptionRecord->NumberParameters << "\n";
}

static void printSapphireInfo(std::ostream& stream) {
    stream
        << "Loader Version: "
        << Loader::get()->getVersion().toString() << " "
        << Loader::get()->getVersionType() << "\n"
        << "Installed mods: " << Loader::get()->getAllMods().size() << "\n"
        << "Failed mods: " << Loader::get()->getFailedMods().size() << "\n";
}

static void printMods(std::ostream& stream) {
    for (auto& mod : Loader::get()->getAllMods()) {
        stream
            << mod->getID() << " | "
            << mod->getDeveloper() << " | "
            << mod->getVersion().toString() << " | "
            << (mod->isEnabled() ? "Enabled" : "Disabled") << " | "
            << (mod->isLoaded() ? "Loaded" : "Unloaded") << " | "
            << "\n";
    }
}

static LONG WINAPI exceptionHandler(LPEXCEPTION_POINTERS info) {
    // make sure crashlog directory exists
    file_utils::createDirectoryAll(crashlog::getCrashLogDirectory());

    // add a file to let Sapphire know on next launch that it crashed previously
    // this could also be done by saving a loader setting or smth but eh.
    file_utils::writeBinary(
        crashlog::getCrashLogDirectory() + "/last-crashed", {}
    );

    std::ofstream file;
    file.open(
        crashlog::getCrashLogDirectory() + "/" + getDateString(true) + ".log",
        std::ios::app
    );

    auto faultyMod = modFromAddress(info->ExceptionRecord->ExceptionAddress);

    file
        << getDateString(false) << "\n"
	    << std::showbase
	    << "Whoopsies! An exception has occurred while running Sapphire.\n";
    
    if (faultyMod) {
        file
            << "It appears that the crash occurred while executing code from "
            << "the \"" << faultyMod->getID() << "\" mod. "
            << "Please submit this crash report to its developer ("
            << faultyMod->getDeveloper() << ") for assistance.\n";
    }
    
    // sapphire info
    file << "\n== Sapphire Information ==\n";
    printSapphireInfo(file);
    
    // exception info
    file << "\n== Exception Information ==\n";
    printInfo(file, info, faultyMod);
    
    // registers
    file << "\n== Register States ==\n";
    printRegisters(file, info->ContextRecord);
    
    // stack trace
    file << "\n== Stack Trace ==\n";
    walkStack(file, info->ContextRecord);

    // mods
    file << "\n== Installed Mods ==\n";
    printMods(file);

    return EXCEPTION_CONTINUE_SEARCH;
}

bool crashlog::setupPlatformHandler() {
    SetUnhandledExceptionFilter(exceptionHandler);
    auto lastCrashedFile = crashlog::getCrashLogDirectory() + "/last-crashed";
    if (ghc::filesystem::exists(lastCrashedFile)) {
        g_lastLaunchCrashed = true;
        try { ghc::filesystem::remove(lastCrashedFile); } catch(...) {}
    }
    return true;
}

bool crashlog::didLastLaunchCrash() {
    return g_lastLaunchCrashed;
}

std::string const& crashlog::getCrashLogDirectory() {
    static auto dir = (
        Loader::get()->getSapphireDirectory() / "crashlogs"
    ).string();
    return dir;
}

#endif
