#include "impl.hpp"

#include <hook.hpp>

using namespace lilac;
using namespace lilac::hook;

hook::Handle SAPPHIRE_CALL lilac::hook::add(const void* address, const void* detour) {
    return HookManager::add_hook(address, detour);
}

bool SAPPHIRE_CALL lilac::hook::remove(hook::Handle handle) {
    return HookManager::remove_hook(handle);
}

bool SAPPHIRE_CALL lilac::hook::write_memory(void* to, void* from, size_t size) {
    return TargetPlatform::write_memory(to, from, size);
}

bool SAPPHIRE_CALL lilac::hook::read_memory(void* from, void* to, size_t size) {
    return TargetPlatform::read_memory(from, to, size);
}

bool SAPPHIRE_CALL lilac::hook::initialize() {
    return TargetPlatform::initialize();
}
