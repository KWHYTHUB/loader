#pragma once

#include <vector>
#include "Platform.hpp"

namespace sapphire::core::impl {
    class MacOSX : public Platform<MacOSX> {
    public:
    	static inline auto trap = { std::byte(0x0f), std::byte(0x0b) };

        static bool writeMemory(void* to, void* from, size_t size);
        static std::vector<std::byte> jump(void* from, void* to);
        static bool initialize();
        static bool enableSingleStep(void* context);
        static bool disableSingleStep(void* context);
        static void* allocateVM(size_t size);
    };

    using TargetPlatform = Platform<MacOSX>;
}
