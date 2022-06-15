#pragma once
#include "Context.hpp"

#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>


namespace BIGGEngine {

/** @struct Implementation of ContextI using GLFW */
namespace GLFWContext {
    void init();
};
}   // namespace BIGGEngine