// BIGGEngine profiling
// Intended to only be included into debug versions. For example:
// Used code from glfwGetTime in files input.c, platform.h, cocoa_time.*, win32_time.*, posix_time.*

//========================================================================
// GLFW 3.4 - www.glfw.org
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2018 Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#pragma once

#if defined(__APPLE__)
    #include <mach/mach_time.h>
#elif defined(_WIN32)
    #include <windows.h>
#else
    #include <time.h>
    #include <unistd.h>
    #include <sys/time.h>
#endif

#include <bx/bx.h>

namespace BIGGEngine {

/** 
 * Static class @c Profile. 
 */
struct Profile {

    Profile() = delete;

public:
    static inline double now() {
        return (double) (getPlatformTime() - s_offset) / s_frequency ;
    }

private:
    static uint64_t getPlatformTime() {
#   if defined(__APPLE__)
        return mach_absolute_time();
#   elif defined(_WIN32)
        uint64_t time;
        QueryPerformanceCounter((LARGE_INTEGER*) &time);
        return time;
#   else
        struct timespec ts;
        clock_gettime(s_clockid, &ts);
        return = ts.tv_sec * 1000000000 + (uint64_t) ts.tv_nsec;
#   endif
    }

    static uint64_t getPlatformFrequency() {
#   if defined(__APPLE__)
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        return (info.denom * 1e9) / info.numer;   // in seconds
#   elif defined(_WIN32)
        uint64_t frequency;
        QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
        return frequency;
#   else
        return 1000000000;
#   endif
    }

    static uint64_t getPlatformClockID() {
#   if defined(__APPLE__) || defined(_WIN32)
        return 0;
#   else
        uint64_t clockid = CLOCK_REALTIME;
#       if defined(_POSIX_MONOTONIC_CLOCK)
            struct timespec ts;
            if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
                clockid = CLOCK_MONOTONIC;
#       endif
        return clockid;
#   endif
    }

    static const inline uint64_t s_offset = getPlatformTime();
    static const inline uint64_t s_frequency = getPlatformFrequency();
    /** Not used on WIN32 or Apple systems. */
    static const inline uint64_t s_clockid = getPlatformClockID();
};

template <typename Func>
struct ScopedTimer {

    ScopedTimer(Func func) : m_func(func) {
        m_startTime = Profile::now();
    }

    ~ScopedTimer() {
        // call the callback with how long this scope existed for.
        m_func(m_startTime, Profile::now() - m_startTime);
    }

private:
    double m_startTime;
    Func m_func;
};

#define _BIGG_CAT_1(x, y) x ## y
#define _BIGG_CAT_2(x, y) _BIGG_CAT_2(x, y)

#define _BIGG_PROFILE_CATEGORY_NAME(_line, _category, _name) BIGGEngine::ScopedTimer _BIGG_CAT_1(timer, _line)([&, prettyFunction = BX_FUNCTION](double startTime, double delta) {BIGGEngine::Log::m_profileLogger->debug("\"cat\": \"{:s}\", \"ts\": {:f}, \"dur\": {:f}, \"name\": \"{:s}\"", _category, startTime*1e6, delta*1e6, _name);})
#define _BIGG_PROFILE_CATEGORY_FUNCTION(_category)              _BIGG_PROFILE_CATEGORY_NAME(__LINE__, _category, prettyFunction)
#define _BIGG_PROFILE_CATEGORY_CUSTOM(_category, _format, ...)  _BIGG_PROFILE_CATEGORY_NAME(__LINE__, _category, fmt::format(_format, ##__VA_ARGS__))
#define _BIGG_PROFILE_CATEGORY_SCOPE(_category, _format, ...)   _BIGG_PROFILE_CATEGORY_CUSTOM(_category, fmt::format("{} {}", prettyFunction, _format), ##__VA_ARGS__)
// _BIGG_PROFILE_CATEGORY_SCOPE("run", "Events Type: {:s}",GET_EVENT_DEBUG_STRING(event))

//common categories
#define BIGG_PROFILE_INIT_FUNCTION                  _BIGG_PROFILE_CATEGORY_FUNCTION("init")
#define BIGG_PROFILE_INIT_SCOPE(_format, ...)       _BIGG_PROFILE_CATEGORY_SCOPE("init", _format, ##__VA_ARGS__)
#define BIGG_PROFILE_RUN_FUNCTION                  _BIGG_PROFILE_CATEGORY_FUNCTION("run")
#define BIGG_PROFILE_RUN_SCOPE(_format, ...)       _BIGG_PROFILE_CATEGORY_SCOPE("run", _format, ##__VA_ARGS__)
#define BIGG_PROFILE_SHUTDOWN_FUNCTION              _BIGG_PROFILE_CATEGORY_FUNCTION("shutdown")
#define BIGG_PROFILE_SHUTDOWN_SCOPE(_format, ...)   _BIGG_PROFILE_CATEGORY_SCOPE("shutdown", _format, ##__VA_ARGS__)

} // namespace BIGGEngine