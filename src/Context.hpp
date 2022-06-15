#pragma once

#include <bx/allocator.h>   // for bx::AllocatorI

namespace BIGGEngine {
namespace Context {

    void init();        // new m_allocator
    void shutdown();    // delete m_allocator

    // an implementation must implement these functions:

    int run();
    void* getNativeWindowHandle();
    void* getNativeDisplayHandle();

    glm::ivec2 getWindowSize();
    glm::ivec2 getWindowFramebufferSize();
    glm::vec2 getWindowContentScale();
    glm::ivec2 getWindowPosition();
    bool getWindowIconified();
    bool getWindowMaximized();
    bool getWindowVisible();
    bool getWindowFocus();
    ActionEnum getKey(KeyEnum key);
    glm::dvec2 getMousePosition();
    bool getMouseHover();
    ActionEnum getMouseButton(MouseButtonEnum button);
    std::string getClipboardString();

    void setWindowShouldClose(bool shouldClose);
    void setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight);
    void setWindowAspectRatio(int numerator, int denominator);
    void setWindowTitle(std::string title);
    void setWindowVisible(bool visible);
    void setClipboardString(std::string text);

    // TODO set window icon
    // TODO get/set monitor???

    // ------------ end implementation functions ---------------


    bx::AllocatorI* getAllocator();
};
}   //namespace BIGGEngine