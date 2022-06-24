//
// Created by Stefan Antoszko on 2022-06-23.
//

#pragma once
#include <bgfx/bgfx.h>

#define BIGG_PROFILE_RENDER_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("render")
#define BIGG_PROFILE_RENDER_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("render", _format, ##__VA_ARGS__)

namespace BIGGEngine {
namespace RenderUtils {

    const bgfx::Memory* loadMem(bx::AllocatorI* allocator, const char* filepath);

} // namespace RenderBase
} // namespace BIGGEngine




