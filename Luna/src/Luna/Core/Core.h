#pragma once

#include <memory>

#ifdef LUNA_PLATFORM_WINDOWS
    #define LUNA_DEBUGBREAK() __debugbreak()
#elif defined(LUNA_PLATFORM_LINUX)
    #include <csignal>
    #define LUNA_DEBUGBREAK() raise(SIGTRAP)
#else
    #define LUNA_DEBUGBREAK() LUNA_CORE_ERROR("No DebugBreak Implementation")
#endif

#ifdef LUNA_ENABLE_ASSERTS
    #define LUNA_ASSERT(x, ...) { if(!(x)) { LUNA_ERROR("Assertion Failed: {0}", __VA_ARGS__); LUNA_DEBUGBREAK(); } }
    #define LUNA_CORE_ASSERT(x, ...) { if(!(x)) { LUNA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); LUNA_DEBUGBREAK(); } }
#else
    #define LUNA_ASSERT(x, ...)
    #define LUNA_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define LUNA_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Luna {

    template <typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
	    return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}
