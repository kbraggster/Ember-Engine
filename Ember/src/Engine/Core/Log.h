#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Ember
{

class Log
{
  public:
    static void Init();

    static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

  private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};

} // namespace Ember

// Core log macros
#define EM_CORE_TRACE(...) Ember::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define EM_CORE_INFO(...) Ember::Log::GetCoreLogger()->info(__VA_ARGS__)
#define EM_CORE_WARN(...) Ember::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define EM_CORE_ERROR(...) Ember::Log::GetCoreLogger()->error(__VA_ARGS__)
#define EM_CORE_CRITICAL(...) Ember::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define EM_TRACE(...) Ember::Log::GetClientLogger()->trace(__VA_ARGS__)
#define EM_INFO(...) Ember::Log::GetClientLogger()->info(__VA_ARGS__)
#define EM_WARN(...) Ember::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EM_ERROR(...) Ember::Log::GetClientLogger()->error(__VA_ARGS__)
#define EM_CRITICAL(...) Ember::Log::GetClientLogger()->critical(__VA_ARGS__)