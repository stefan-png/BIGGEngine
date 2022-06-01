#pragma once

#include <stdint.h> // int8_t

namespace BIGGEngine {

/// Engine constants
const double        g_ticksFrequency    = 20.0;
const double        g_tickDeltaTime     = 1 / g_ticksFrequency;

// const priority levels
const int8_t        g_contextPriority       = -128;
const int8_t        g_rendererBeginPriority = -127;
const int8_t        g_rendererEndPriority   = 127;
const int8_t        g_renderUIBeginPriority = -126;
const int8_t        g_renderUIEndPriority   = 126;
const int8_t        g_renderMeshComponentsPriority  = 125;

/// Context constants
const unsigned int  g_maxWindowCount    = 20;

/// Renderer constants
const bool          g_vSyncEnabled      = false;

} // namespace BIGGEngine