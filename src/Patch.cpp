#include <loader/Hook.hpp>
#include <vector>
#include <loader/Mod.hpp>
#include <loader/Loader.hpp>
#include <utils/casts.hpp>
#include <utils/vector.hpp>
// #include <hook/hook.hpp>
#include "Internal.hpp"

USE_GEODE_NAMESPACE();

Result<Patch*> Mod::patch(void* address, byte_array data) {
    auto p = new Patch;
    p->m_address = address;
    p->m_original = byte_array(data.size());
    // if (!geode::core::hook::read_memory(address, p->m_original.data(), data.size())) {
    //     delete p;
    //     return Err<>("Unable to read memory at " + std::to_string(p->getAddress()));
    // }
    p->m_owner = this;
    p->m_patch = data;
    if (!p->apply()) {
        delete p;
        return Err<>("Unable to enable patch at " + std::to_string(p->getAddress()));
    }
    this->m_patches.push_back(p);
    return Ok<Patch*>(p);
}

Result<> Mod::unpatch(Patch* patch) {
    if (patch->restore()) {
        vector_utils::erase<Patch*>(this->m_patches, patch);
        delete patch;
        return Ok<>();
    }
    return Err<>("Unable to restore patch!");
}

bool Patch::apply() {
	return true;
    // return geode::core::hook::write_memory(
    //     this->m_address, this->m_patch.data(), this->m_patch.size()
    // );
}

bool Patch::restore() {
	return true;
    // return geode::core::hook::write_memory(
    //     this->m_address, this->m_original.data(), this->m_original.size()
    // );
}
