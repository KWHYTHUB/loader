#include "../crashlog.hpp"

#ifdef GEODE_IS_MACOS

bool crashlog::setupPlatformHandler() {
    return false;
}

bool crashlog::didLastLaunchCrash() {
    return false;
}

std::string const& crashlog::getCrashLogDirectory() {
    return "";
}

#endif