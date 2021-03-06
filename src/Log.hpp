// defining SPDLOG_ACTIVE_LEVEL will set the level at a compiler level (ie. should be TRACE for a shared lib)
// need to also set spdlog::set_level() based on NDEBUG
#pragma once

#if defined(NDEBUG)
    #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#else
    #if !defined(SPDLOG_ACTIVE_LEVEL)
        #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
    #endif
#endif

#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <memory>   // for shared_ptr

#include <spdlog/fmt/chrono.h>  // so that can sent a std::tm into a fmt::format

/** @def BIGG_LOG_TRACE 
 * Log a trace message to @c std::cout. Formatted using @c spdlog */
/** @def BIGG_LOG_DEBUG 
 * Log a debug message to @c std::cout. Formatted using @c spdlog */
/** @def BIGG_LOG_INFO 
 * Log an info message to @c std::cout. Formatted using @c spdlog */
/** @def BIGG_LOG_WARN 
 * Log a warning message to @c std::cout. Formatted using @c spdlog */
/** @def BIGG_LOG_CRITICAL 
 * Log a critical message to @c std::cout. Formatted using @c spdlog */

#define BIGG_LOG_TRACE(...) SPDLOG_LOGGER_TRACE(BIGGEngine::Log::m_appLogger, __VA_ARGS__)
#define BIGG_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(BIGGEngine::Log::m_appLogger, __VA_ARGS__)
#define BIGG_LOG_INFO(...) SPDLOG_LOGGER_INFO(BIGGEngine::Log::m_appLogger, __VA_ARGS__)
#define BIGG_LOG_WARN(...) SPDLOG_LOGGER_WARN(BIGGEngine::Log::m_appLogger, __VA_ARGS__)
#define BIGG_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(BIGGEngine::Log::m_appLogger, __VA_ARGS__)
#define BIGG_ASSERT(_condition, _format, ...)                                           \
    for(;;) {                                                                           \
        if(!(_condition)) {                                                             \
            SPDLOG_LOGGER_CRITICAL(Log::m_appLogger, "ASSERT " _format, ##__VA_ARGS__); \
            system("pwd");                                                              \
            abort();                                                                    \
        }                                                                               \
    break; }

namespace BIGGEngine {
/** 
 * Static class @c Log. 
 */
struct Log {
    Log() = delete;

    enum struct LogLevel {
        Trace = SPDLOG_LEVEL_TRACE,
        Debug = SPDLOG_LEVEL_DEBUG,
        Info = SPDLOG_LEVEL_INFO,
        Warn = SPDLOG_LEVEL_WARN,
        Critical = SPDLOG_LEVEL_CRITICAL
    };

    static void init() {

        m_appLogger     = spdlog::stdout_color_mt("APP");
        m_contextLogger = spdlog::stdout_color_mt("CTX");
        m_scriptLogger = spdlog::stdout_color_mt("LUA");
        spdlog::set_default_logger(m_appLogger);
        spdlog::set_pattern("[%=3!n] [%T.%e] %10s:%-3# %^[%l] %v %$");
        

        if(spdlog::get_level() >= spdlog::level::level_enum::info) {
            m_profileLogger = spdlog::null_logger_mt("PRF");
        } else {
            const std::time_t now = std::time(nullptr);
            const std::tm* cnow = std::localtime(&now);
            m_profileLogger = spdlog::basic_logger_mt("PROFILE", fmt::format("../log/{:%Y-%m-%d_%H-%M-%S}_profile.json", *cnow), true);
            m_profileLogger->set_pattern("[");
            m_profileLogger->debug("");
            m_profileLogger->set_pattern("{\"ph\": \"X\", \"pid\": %P, \"tid\": %t, %v },");
        }
    }

    static inline std::shared_ptr<spdlog::logger> m_appLogger;
    static inline std::shared_ptr<spdlog::logger> m_contextLogger;
    static inline std::shared_ptr<spdlog::logger> m_profileLogger;
    static inline std::shared_ptr<spdlog::logger> m_scriptLogger;

    static void setLevel(LogLevel level) {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
    }
    static LogLevel getLevel() {
//        spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
    }
};
}   // namespace BiggEngine

//  ------------------------ GLM FORMATTERS ------------------------------------
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#if GLM_HAS_TEMPLATE_ALIASES

template<typename T>
struct fmt::formatter<glm::tvec1<T>> : formatter<T> {
    template<typename FormatContext>
    constexpr auto format(const glm::tvec1<T> &t, FormatContext &ctx) -> decltype(ctx.out()) {
        return fmt::formatter<T>::format(t.x, ctx);
    }
};  // glm::tvec1<T>
template<typename T>
struct fmt::formatter<glm::tvec2<T>> : formatter<T> {
    template<typename FormatContext>
    constexpr auto format(const glm::tvec2<T> &t, FormatContext &ctx) -> decltype(ctx.out()) {
        fmt::formatter<T>::format(t.x, ctx);
        fmt::format_to(ctx.out(), ", ");
        return fmt::formatter<T>::format(t.y, ctx);
    }
};  // glm::tvec2<T>
template<typename T>
struct fmt::formatter<glm::tvec3<T>> : formatter<T> {
    template<typename FormatContext>
    constexpr auto format(const glm::tvec3<T> &t, FormatContext &ctx) -> decltype(ctx.out()) {
        fmt::formatter<T>::format(t.x, ctx);
        fmt::format_to(ctx.out(), ", ");
        fmt::formatter<T>::format(t.y, ctx);
        fmt::format_to(ctx.out(), ", ");
        return fmt::formatter<T>::format(t.z, ctx);
    }
};  // glm::tvec3<T>
template<typename T>
struct fmt::formatter<glm::tvec4<T>> : formatter<T> {
    template<typename FormatContext>
    constexpr auto format(const glm::tvec4<T> &t, FormatContext &ctx) -> decltype(ctx.out()) {
        fmt::formatter<T>::format(t.x, ctx);
        fmt::format_to(ctx.out(), ", ");
        fmt::formatter<T>::format(t.y, ctx);
        fmt::format_to(ctx.out(), ", ");
        fmt::formatter<T>::format(t.z, ctx);
        fmt::format_to(ctx.out(), ", ");
        return fmt::formatter<T>::format(t.w, ctx);
    }
};  // glm::tvec4<T>
template<typename T>
struct fmt::formatter<glm::tmat4x4<T>> : formatter<typename glm::tmat4x4<T>::col_type> {
    template<typename FormatContext>
    constexpr auto format(const glm::tmat4x4<T> &t, FormatContext &ctx) -> decltype(ctx.out()) {
        using col_type = typename glm::tmat4x4<T>::col_type;
        fmt::format_to(ctx.out(), "\n");
        fmt::formatter<col_type>::format(t[0], ctx);
        fmt::format_to(ctx.out(), "\n");
        fmt::formatter<col_type>::format(t[1], ctx);
        fmt::format_to(ctx.out(), "\n");
        fmt::formatter<col_type>::format(t[2], ctx);
        fmt::format_to(ctx.out(), "\n");
        return fmt::formatter<col_type>::format(t[3], ctx);
    }
};  // glm::tmat4x4<T>
template<typename T>
struct fmt::formatter<glm::tquat<T>> : formatter<T> {
    template<typename FormatContext>
    constexpr auto format(const glm::tquat<T> &t, FormatContext &ctx) -> decltype(ctx.out()) {
        fmt::formatter<T>::format(t.w, ctx);
        fmt::format_to(ctx.out(), " + ");
        fmt::formatter<T>::format(t.x, ctx);
        fmt::format_to(ctx.out(), "i + ");
        fmt::formatter<T>::format(t.y, ctx);
        fmt::format_to(ctx.out(), "j + ");
        fmt::formatter<T>::format(t.z, ctx);
        return fmt::format_to(ctx.out(), "k");
    }
};  // glm::tquat<T>

#endif  // GLM_HAS_TEMPLATE_ALIASES