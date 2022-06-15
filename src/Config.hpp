#pragma once

#include <stdint.h> // uint16_t, UINT16_MAX

namespace BIGGEngine {

/// Engine constants
const double        g_ticksFrequency    = 20.0;
const double        g_tickDeltaTime     = 1 / g_ticksFrequency;

// const priority levels
const uint16_t        g_contextPriority       = 0;
const uint16_t        g_renderBaseBeginPriority = 1;
const uint16_t        g_renderBaseEndPriority   = UINT16_MAX;
const uint16_t        g_renderUIBeginPriority = 2;
const uint16_t        g_renderUIEndPriority   = UINT16_MAX-1;
const uint16_t        g_renderMeshComponentsPriority  = UINT16_MAX-2;

/// Context constants
const unsigned int  g_maxWindowCount    = 20;

/// Renderer constants
const bool          g_vSyncEnabled      = false;

} // namespace BIGGEngine