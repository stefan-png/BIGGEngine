#pragma once
#include "ContextI.hpp"

#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>


namespace BIGGEngine {

/** @struct Implementation of ContextI using GLFW */
struct GLFWContext : public ContextI {

    GLFWContext();
    GLFWContext(bx::AllocatorI* allocator);
    ~GLFWContext();

    /**
     * This is the main loop of the program. It blocks the main thread.
     */
    int run() override;

    /**
     * @defgroup VirtualGettersAndSettersGLFWImpl Overridden getters and setters. 
     * @{
     */
    void* getNativeWindowHandle() override;
    void* getNativeDisplayHandle() override;

    glm::ivec2 getWindowSize() override;
    glm::ivec2 getWindowFramebufferSize() override;
    glm::vec2 getWindowContentScale() override;
    glm::ivec2 getWindowPosition() override;
    bool getWindowIconified() override;
    bool getWindowMaximized() override;
    bool getWindowVisible() override;
    bool getWindowFocus() override;
    ActionEnum getKey(KeyEnum key) override;
    glm::dvec2 getMousePosition() override;
    bool getMouseHover() override;
    ActionEnum getMouseButton(MouseButtonEnum button) override;
    std::string getClipboardString() override;

    void setWindowShouldClose(bool shouldClose) override;
    /** Can set any parameter to @c GLFW_DONT_CARE aka @c -1  */
    void setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) override;
    /** Can set any parameter to @c GLFW_DONT_CARE aka @c -1  */
    void setWindowAspectRatio(int numerator, int denominator) override;
    void setWindowTitle(std::string title) override;
    void setWindowVisible(bool visible) override;
    void setClipboardString(std::string text) override;
    /** @} */   // end VirtualGettersAndSettersGLFWImpl

protected:
    /**
     * @defgroup VirtualHandlersGLFWImpl Overridden handlers. 
     * @{
     */
    bool handleWindowCreation(WindowCreateEvent* event) override;
    bool handleWindowDestruction(WindowDestroyEvent* event) override;
    bool handleWindowSize(WindowSizeEvent* event) override;
    bool handleWindowPosition(WindowPositionEvent* event) override;
    bool handleWindowIconified(WindowIconifyEvent* event) override;
    bool handleWindowMaximized(WindowMaximizeEvent* event) override;
    bool handleWindowFocus(WindowFocusEvent* event) override;
    /** @} */   // end VirtualHandlersGLFWImpl

private:
    /** 
     * @todo this should be an array of size MAX_WINDOWS
     */ 
    GLFWwindow* m_window = nullptr;

};
}   // namespace BIGGEngine