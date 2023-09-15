#ifndef SAPPHIRE_CORE_META_PREPROC_HPP
#define SAPPHIRE_CORE_META_PREPROC_HPP

#if !defined(SAPPHIRE_CALL)
    // Windows
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        #define SAPPHIRE_WINDOWS(...) __VA_ARGS__
        #define SAPPHIRE_IS_WINDOWS
        #define SAPPHIRE_IS_DESKTOP
        #define SAPPHIRE_PLATFORM_NAME "Windows"
        #define SAPPHIRE_PLATFORM_TARGET PlatformID::Windows
        #define SAPPHIRE_CALL __stdcall
        #define SAPPHIRE_PLATFORM_EXTENSION ".dll"
    #else
        #define SAPPHIRE_WINDOWS(...)
    #endif

	// Darwin
	#if defined(__APPLE__)
		#include <TargetConditionals.h>
		#if TARGET_OS_IPHONE
			#define SAPPHIRE_MACOS(...)
			#define SAPPHIRE_IOS(...) __VA_ARGS__
			#define SAPPHIRE_IS_IOS
			#define SAPPHIRE_IS_MOBILE
			#define SAPPHIRE_PLATFORM_NAME "iOS"
			#define SAPPHIRE_PLATFORM_TARGET PlatformID::iOS
			#define SAPPHIRE_PLATFORM_EXTENSION ".dylib"
		#else
			#define SAPPHIRE_IOS(...)
			#define SAPPHIRE_MACOS(...) __VA_ARGS__
			#define SAPPHIRE_IS_MACOS
			#define SAPPHIRE_IS_DESKTOP
			#define SAPPHIRE_PLATFORM_NAME "MacOS"
			#define SAPPHIRE_PLATFORM_TARGET PlatformID::MacOS
			#define SAPPHIRE_PLATFORM_EXTENSION ".dylib"
		#endif
		#define SAPPHIRE_CALL
	#else
		#define SAPPHIRE_MACOS(...)
		#define SAPPHIRE_IOS(...)
	#endif

    // Android
    #if defined(__ANDROID__)
        #define SAPPHIRE_ANDROID(...) __VA_ARGS__
        #define SAPPHIRE_IS_ANDROID
        #define SAPPHIRE_IS_MOBILE
        #define SAPPHIRE_PLATFORM_NAME "Android"
        #define SAPPHIRE_PLATFORM_TARGET PlatformID::Android
        #define SAPPHIRE_CALL
        #define SAPPHIRE_PLATFORM_EXTENSION ".so"
    #else
        #define SAPPHIRE_ANDROID(...)
    #endif

    #ifndef SAPPHIRE_PLATFORM_NAME
        #error "Unsupported PlatformID!"
    #endif

    #ifdef SAPPHIRE_EXPORTING
        #define SAPPHIRE_DLL SAPPHIRE_WINDOWS(__declspec(dllexport))
    #else
        #define SAPPHIRE_DLL SAPPHIRE_WINDOWS(__declspec(dllimport))
    #endif
    #define SAPPHIRE_API SAPPHIRE_WINDOWS() // idk where this is
#endif

#endif /* SAPPHIRE_CORE_META_PREPROC_HPP */
