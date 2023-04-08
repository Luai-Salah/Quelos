#pragma once

#include "spdlog\spdlog.h"
#include "spdlog\fmt\ostr.h"

namespace Quelos
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define QS_CORE_ERROR(...) ::Quelos::Log::GetCoreLogger()->error(__VA_ARGS__)
#define QS_CORE_TRACE(...) ::Quelos::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define QS_CORE_INFO(...) ::Quelos::Log::GetCoreLogger()->info(__VA_ARGS__)
#define QS_CORE_WARN(...) ::Quelos::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define QS_CORE_CRITICAL(...) ::Quelos::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define QS_ERROR(...) ::Quelos::Log::GetClientLogger()->error(__VA_ARGS__)
#define QS_TRACE(...) ::Quelos::Log::GetClientLogger()->trace(__VA_ARGS__)
#define QS_LOG(...) ::Quelos::Log::GetClientLogger()->info(__VA_ARGS__)
#define QS_WARN(...) ::Quelos::Log::GetClientLogger()->warn(__VA_ARGS__)
#define QS_CRITICAL(...) ::Quelos::Log::GetClientLogger()->critical(__VA_ARGS__)
