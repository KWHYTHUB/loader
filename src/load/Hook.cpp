#include <loader/Hook.hpp>
#include <vector>
#include <loader/Mod.hpp>
#include <loader/Loader.hpp>
#include <utils/casts.hpp>
#include <utils/vector.hpp>
// #include <hook/hook.hpp>
#include <hook-core/Hook.hpp>
#include "InternalLoader.hpp"
#include "InternalMod.hpp"

USE_SAPPHIRE_NAMESPACE();

struct hook_info {
    Hook* hook;
    Mod* mod;
};

// for some reason this doesn't work as 
// a normal static global. the vector just 
// gets cleared for no reason somewhere 
// between `addHook` and `loadHooks`


SAPPHIRE_STATIC_VAR(std::vector<hook_info>, internalHooks);
SAPPHIRE_STATIC_VAR(bool, readyToHook);


Result<> Mod::enableHook(Hook* hook) {
    if (!hook->isEnabled()) {
    	auto res = std::invoke(hook->m_addFunction, hook->m_address);
	    if (res) {
	        this->m_hooks.push_back(hook);
	        hook->m_enabled = true;
	        hook->m_handle = res.value();
	        return Ok<>();
		} else {
	        return Err<>(
	            "Unable to create hook at " + std::to_string(as<uintptr_t>(hook->m_address))
	        );
	    }
	    return Err<>("Hook already has a handle");
    }
    return Ok<>();
}

Result<> Mod::disableHook(Hook* hook) {
    if (hook->isEnabled()) {
        if (sapphire::core::hook::remove(hook->m_handle)) {
            hook->m_enabled = false;
            return Ok<>();
        }
        return Err<>("Unable to remove hook");
    }
    return Ok<>();
}

Result<> Mod::removeHook(Hook* hook) {
    auto res = this->disableHook(hook);
    if (res) {
        vector_utils::erase<Hook*>(this->m_hooks, hook);
        delete hook;
    }
    return res;
}

Result<Hook*> Mod::addHook(Hook* hook) {
    if (readyToHook()) {
        auto res = this->enableHook(hook);
		if (!res) {
			delete hook;
			return Err<>("Can't create hook");
		}
    }
    else {
    	internalHooks().push_back({ hook, this });
    }
    return Ok<Hook*>(hook);
}

bool InternalLoader::loadHooks() {
    readyToHook() = true;
    auto thereWereErrors = false;
    for (auto const& hook : internalHooks()) {
        auto res = hook.mod->addHook(hook.hook);
        if (!res) {
            hook.mod->logInfo(
                res.error(),
                Severity::Error
            );
            thereWereErrors = true;
        }
    }
    // free up memory
    internalHooks().clear();
    return !thereWereErrors;
}
