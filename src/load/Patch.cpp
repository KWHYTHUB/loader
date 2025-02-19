#include <loader/Hook.hpp>
#include <vector>
#include <loader/Mod.hpp>
#include <loader/Loader.hpp>
#include <utils/casts.hpp>
#include <utils/vector.hpp>
#include <InternalLoader.hpp>

#include <lilac/include/sapphire/core/hook/hook.hpp>

USE_SAPPHIRE_NAMESPACE();

byte_array readMemory(void* address, size_t amount) {
    byte_array ret;
    for (size_t i = 0; i < amount; i++) {
        ret.push_back(*as<uint8_t*>(as<uintptr_t>(address) + i));
    }
    return ret;
}

Result<Patch*> Mod::patch(void* address, byte_array data) {
    auto p = new Patch;
    p->m_address = address;
    p->m_original = readMemory(address, data.size());
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
    return lilac::hook::write_memory(
         this->m_address, this->m_patch.data(), this->m_patch.size()
    );
}

bool Patch::restore() {
    return lilac::hook::write_memory(
        this->m_address, this->m_original.data(), this->m_original.size()
    );
}
