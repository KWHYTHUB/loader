#ifndef SAPPHIRE_CORE_META_CDECL_HPP
#define SAPPHIRE_CORE_META_CDECL_HPP

namespace lilac::meta::x86 {
    template <class Ret, class... Args>
    class Cdecl {
    private:
        template <Ret (*detour)(Args...)>
        static Ret __cdecl wrapper(Args... all) {
            return detour(all...);
        }

    public:
        static Ret invoke(void* address, Args... all) {
            Ret(__cdecl * raw)(Args...) = reinterpret_cast<decltype(raw)>(address);
            return raw(all...);
        }

        template <Ret (*detour)(Args...)>
        static constexpr decltype(auto) get_wrapper() {
            return &wrapper<detour>;
        }
    };
}

#endif /* SAPPHIRE_CORE_META_THISCALL_HPP */
