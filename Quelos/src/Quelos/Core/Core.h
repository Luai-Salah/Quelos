#pragma once

#include <memory>

#ifdef _WIN32
	#ifndef _WIN64
		#error "x86 Builds are not supported!"
	#endif // _WIN64
#endif

#ifdef QS_DEBUG
	#ifdef QS_PLATFORM_WINDOWS
		#define QS_DEBUG_BREAK() __debugbreak()
	#endif // QS_PLATFORM_WINDOWS
#endif // QS_DEBUG


#define BIT(x) (1 << x)
#define BIND_EVNT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

#ifdef QS_ENABLE_ASSERT
	#define QS_CORE_ASSERT(x, ...) {if(!(x)) {QS_CORE_CRITICAL("Assertion failed-> {0}", __VA_ARGS__); QS_DEBUG_BREAK();}}
	#define QS_ASSERT(x, ...) {if(!(x)) {QS_CRITICAL("Assertion failed-> {0}", __VA_ARGS__); QS_DEBUG_BREAK();}}
#else
	#define QS_CORE_ASSERT(x, ...)
	#define QS_ASSERT(x, ...)
#endif // QS_ENABLE_ASSERT

namespace Quelos
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<class T, typename... TArgs>
	Scope<T> CreateScope(TArgs&&... _args)
	{
		return std::make_unique<T>(std::forward<TArgs>(_args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<class T, typename... TArgs>
	Ref<T> CreateRef(TArgs&&... _args)
	{
		return std::make_shared<T>(std::forward<TArgs>(_args)...);
	}
}
