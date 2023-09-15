#include <utils/platform.hpp>

#ifdef SAPPHIRE_IS_IOS

#include <Sapphire.hpp>
#include <dlfcn.h>

USE_SAPPHIRE_NAMESPACE();

template<typename T>
T findSymbolOrMangled(void* dylib, const char* name, const char* mangled) {
	auto res = reinterpret_cast<T>(dlsym(dylib, name));
	if (!res) {
		res = reinterpret_cast<T>(dlsym(dylib,mangled));
	}
	return res;
}

Result<> Mod::loadPlatformBinary() {
	auto dylib = dlopen((this->m_tempDirName / this->m_info.m_binaryName).string().c_str(), RTLD_LAZY);
	if (dylib) {
		this->m_implicitLoadFunc = findSymbolOrMangled<sapphire_load>(dylib, "sapphire_implicit_load", "_sapphire_implicit_load");
		this->m_loadFunc = findSymbolOrMangled<sapphire_load>(dylib, "sapphire_load", "_sapphire_load");
		this->m_unloadFunc = findSymbolOrMangled<sapphire_unload>(dylib, "sapphire_unload", "_sapphire_unload");
		this->m_enableFunc = findSymbolOrMangled<sapphire_enable>(dylib, "sapphire_enable", "_sapphire_enable");
		this->m_disableFunc = findSymbolOrMangled<sapphire_disable>(dylib, "sapphire_disable", "_sapphire_disable");
		this->m_saveDataFunc = findSymbolOrMangled<sapphire_save_data>(dylib, "sapphire_save_data", "_sapphire_save_data");
		this->m_loadDataFunc = findSymbolOrMangled<sapphire_load_data>(dylib, "sapphire_load_data", "_sapphire_load_data");
		this->m_settingUpdatedFunc = findSymbolOrMangled<sapphire_setting_updated>(dylib, "sapphire_setting_updated", "_sapphire_setting_updated");

		if (!this->m_implicitLoadFunc && !this->m_loadFunc) {
			return Err<>("Unable to find mod entry point (lacking both implicit & explicit definition)");
		}

		if (this->m_platformInfo) {
			delete this->m_platformInfo;
		}
		this->m_platformInfo = new PlatformInfo { dylib };

		return Ok<>();
	}
	std::string err = (char const*)dlerror();
	return Err<>("Unable to load the DYLIB: dlerror returned (" + err + ")");
}

Result<> Mod::unloadPlatformBinary() {
	auto dylib = this->m_platformInfo->m_dylib;
	delete this->m_platformInfo;
	this->m_platformInfo = nullptr;
	if (dlclose(dylib) == 0) {
		this->m_unloadFunc = nullptr;
		this->m_loadFunc = nullptr;
		this->m_implicitLoadFunc = nullptr;
		this->m_enableFunc = nullptr;
		this->m_disableFunc = nullptr;
		this->m_saveDataFunc = nullptr;
		this->m_loadDataFunc = nullptr;
		this->m_settingUpdatedFunc = nullptr;
		return Ok<>();
	} else {
		return Err<>("Unable to free library");
	}
}

#endif
