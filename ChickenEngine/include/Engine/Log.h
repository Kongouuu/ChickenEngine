#pragma once

#include "pch.h"

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace ChickenEngine
{
	class CHICKEN_API Log
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

//// :: before namespace means to find the namespace from global namespace, instead of local
//#define LOG_CORE_ERROR(...) ::ChickenEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
//#define LOG_CORE_WARN(...)  ::ChickenEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
//#define LOG_CORE_INFO(...)  ::ChickenEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
//#define LOG_CORE_TRACE(...) ::ChickenEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
//
//#define LOG_CLIENT_ERROR(...) ::ChickenEngine::Log::GetClientLogger()->error(__VA_ARGS__)
//#define LOG_CLIENT_WARN(...)  ::ChickenEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
//#define LOG_CLIENT_INFO(...)  ::ChickenEngine::Log::GetClientLogger()->info(__VA_ARGS__)
//#define LOG_CLIENT_TRACE(...) ::ChickenEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
//

#ifdef _BUILD_DLL
#define LOG_ERROR(...) ::ChickenEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::ChickenEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::ChickenEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::ChickenEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#else
#define LOG_ERROR(...) ::ChickenEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::ChickenEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::ChickenEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::ChickenEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#endif

