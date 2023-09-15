#pragma once

class InternalMod;

#include <loader/Mod.hpp>

USE_SAPPHIRE_NAMESPACE();

class InternalLoader;

class InternalMod : public Mod {
    protected:
        friend class InternalLoade;

        InternalMod();
        virtual ~InternalMod();

    public:
        static InternalMod* get();
};
