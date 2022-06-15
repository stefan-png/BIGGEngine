#pragma once

#include "../Core.hpp"
#include "../Context.hpp"

#include <imgui.h>
#include <bgfx/bgfx.h>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho()
#include <glm/gtc/type_ptr.hpp>         // for glm::value_ptr() (convert mat4 to float[16])

// for reading shader from disk
#include <bx/file.h>
#include <bx/readerwriter.h>

#include <memory>   // for unique_ptr

#define BIGG_PROFILE_UI_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("ui")
#define BIGG_PROFILE_UI_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("ui", _format, ##__VA_ARGS__)

namespace BIGGEngine {


} // namespace BIGGEngine