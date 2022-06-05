#include "Core.hpp"

#include "ContextImplGLFW.hpp"

#if defined(__APPLE__)
#   include "NativeWindowHack.hpp"
#endif

#include <glm/vec2.hpp>

/** logger defines */
#define BIGG_GLFW_LOG_TRACE(...) SPDLOG_LOGGER_TRACE(Log::m_contextLogger, __VA_ARGS__)
#define BIGG_GLFW_LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(Log::m_contextLogger, __VA_ARGS__)
#define BIGG_GLFW_LOG_INFO(...) SPDLOG_LOGGER_INFO(Log::m_contextLogger, __VA_ARGS__)
#define BIGG_GLFW_LOG_WARN(...) SPDLOG_LOGGER_WARN(Log::m_contextLogger, __VA_ARGS__)
#define BIGG_GLFW_LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Log::m_contextLogger, __VA_ARGS__)

#define BIGG_PROFILE_GLFW_FUNCTION                  _BIGG_PROFILE_CATEGORY_FUNCTION("glfw")
#define BIGG_PROFILE_GLFW_SCOPE(_format, ...)       _BIGG_PROFILE_CATEGORY_SCOPE("glfw", _format, ##__VA_ARGS__)

namespace BIGGEngine {

GLFWContext::GLFWContext() : ContextI() {
    /** Since contextLogger is already setup, and I just want to change its name, use @c spdlog::logger::clone() */ 
    Log::m_contextLogger = Log::m_contextLogger->clone("GLFW");
}
GLFWContext::GLFWContext(bx::AllocatorI* allocator) : ContextI(allocator) {}
GLFWContext::~GLFWContext() {}

int GLFWContext::run() {
    {
        BIGG_PROFILE_GLFW_SCOPE("glfwInit()");

        BIGG_GLFW_LOG_DEBUG("Running GLFWContext...");

        glfwSetErrorCallback([](int error, const char* description){
            BIGG_GLFW_LOG_CRITICAL(" GLFW Error Code {:s}: {:s}", error, description);
        });

        if(!glfwInit()) {
            BIGG_GLFW_LOG_CRITICAL("GLFW context failed to initialize!");
        }
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   // important so that window doesn't go black as soon as I move it
    // glfwSwapInterval(1);    //enable vsync
    // poll events once to see if need to create/destroy some windows
    pollEvents();
    // At this point, the only callback which is subscribed should be 
    // the one in ContextI.cpp constructor (ie window create/distroy)
    // Therefore user didn't create a window.
    if(m_window == nullptr) {
        BIGG_GLFW_LOG_WARN("The context has no windows! Try calling GLFWContext::postWindowCreateEvent()");
    }    

    // The way this loop ends up working is that its bottlenecked by how
    // quickly GLFW can poll for events. On my system, mouse events occur
    // every ~8ms or ~11ms with a bluetooth mouse so that is the most rapidly
    // the game will recieve update events. If the mouse isn't moving, the game
    // will recieve an update event every tick, since the world changes at this
    // rate, it makes sense.
    double lastUpdateTime, lastTickTime, timeoutTime, now;
    lastUpdateTime = lastTickTime = now = Profile::now();

    while(m_window != nullptr && !glfwWindowShouldClose(m_window)) {

        {
            BIGG_PROFILE_GLFW_SCOPE("glfwWaitEventsTimeout({:.3f})", g_tickDeltaTime);
            // poll glfw events (which will populate context event queue)
            glfwWaitEventsTimeout(g_tickDeltaTime);    // at least once per tick
        }

        now = Profile::now();

        // post an update event
        postUpdateEvent(now - lastUpdateTime);
        lastUpdateTime = now;

        // post a tick event 
        if(now - lastTickTime >= g_tickDeltaTime) {
            postTickEvent(now - lastTickTime);
            lastTickTime = now;
        }

        // poll context events
        pollEvents();

    }
    BIGG_GLFW_LOG_DEBUG("Terminating GLFWContext...");

    glfwTerminate();
    return EXIT_SUCCESS;   // return from main
}

// Events Callbacks to be overidden by the implementation
bool GLFWContext::handleWindowCreation(WindowCreateEvent* event) {
    BIGG_PROFILE_GLFW_FUNCTION;
    // create glfw window
    if(m_window != nullptr) {
        BIGG_GLFW_LOG_WARN("Tried to create a window when one already exists!");
        return false;
    }

    m_window = glfwCreateWindow(event->m_size.x, event->m_size.y, event->m_title.c_str(), nullptr, nullptr);
    if(m_window == nullptr) {
        
        BIGG_GLFW_LOG_WARN("Failed to create a window!");
        return false;
    }

    // set the user pointer to be the application's context
    glfwSetWindowUserPointer(m_window, this);
    
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowShouldCloseEvent* event = BX_NEW(ctx->getAllocator(), WindowShouldCloseEvent);
        ctx->pushEvent(event);

        BIGG_GLFW_LOG_TRACE("Pushed window close event");
    });
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowSizeEvent* event = BX_NEW(ctx->getAllocator(), WindowSizeEvent);
        event->m_size = { width, height };
        ctx->pushEvent(event);

        ctx->pollEvents();  // propogate the event immediately
        BIGG_GLFW_LOG_TRACE("Pushed & polled window size event");
    });
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowFramebufferSizeEvent* event = BX_NEW(ctx->getAllocator(), WindowFramebufferSizeEvent);
        event->m_size = { width, height };
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed window frame size event");
    });
    glfwSetWindowContentScaleCallback(m_window, [](GLFWwindow* window, float xScale, float yScale){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowContentScaleEvent* event = BX_NEW(ctx->getAllocator(), WindowContentScaleEvent);
        event->m_scale = { xScale, yScale};
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed window content scale event");
    });
    glfwSetWindowPosCallback(m_window, [](GLFWwindow* window, int x, int y){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowPositionEvent* event = BX_NEW(ctx->getAllocator(), WindowPositionEvent);
        event->m_position = { x, y };
        ctx->pushEvent(event);

        ctx->pollEvents();  // propogate the event immediately
        BIGG_GLFW_LOG_TRACE("Pushed & polled window position event");
    });
    glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int iconified){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowIconifyEvent* event = BX_NEW(ctx->getAllocator(), WindowIconifyEvent);
        event->m_iconified = iconified;
        ctx->pushEvent(event);

        ctx->pollEvents();  // propogate the event immediately
        BIGG_GLFW_LOG_TRACE("Pushed & polled window iconify event");
    });
    glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, int maximized){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowMaximizeEvent* event = BX_NEW(ctx->getAllocator(), WindowMaximizeEvent);
        event->m_maximized = maximized;
        ctx->pushEvent(event);

        ctx->pollEvents();  // propogate the event immediately
        BIGG_GLFW_LOG_TRACE("Pushed & polled window maximize event");
    });
    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focus){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowFocusEvent* event = BX_NEW(ctx->getAllocator(), WindowFocusEvent);
        event->m_focused = focus;
        ctx->pushEvent(event);

        ctx->pollEvents();  // propogate the event immediately
        BIGG_GLFW_LOG_TRACE("Pushed & polled window focus event");
    });
    glfwSetWindowRefreshCallback(m_window, [](GLFWwindow* window){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        WindowRefreshEvent* event = BX_NEW(ctx->getAllocator(), WindowRefreshEvent);
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed window refresh event");
    });
    // key callbacks
    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        KeyEvent* event = BX_NEW(ctx->getAllocator(), KeyEvent);
        event->m_key = (KeyEnum)key;
        event->m_scancode = scancode;
        event->m_action = (ActionEnum)action;
        event->m_mods = (ModsEnum)mods;
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed key event");
    });
    glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint){
        static GLFWContext* ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        CharEvent* event = BX_NEW(ctx->getAllocator(), CharEvent);
        event->m_codepoint = codepoint;
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed char event");
    });
    // mouse callbacks
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
        static GLFWContext *const ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));

        static glm::dvec2 lastMousePos;
        glm::dvec2 currentMousePos(x, y);

        MousePositionEvent* event =  BX_NEW(ctx->getAllocator(), MousePositionEvent);
        event->m_delta = currentMousePos - lastMousePos;
        event->m_mousePosition = currentMousePos;
        ctx->pushEvent(event);

        lastMousePos.x = x;
        lastMousePos.y = y;
        BIGG_GLFW_LOG_TRACE("Pushed mouse position event"); // trace since its the most annoying
    });
    glfwSetCursorEnterCallback(m_window, [](GLFWwindow* window, int entered) {
        static GLFWContext *const ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));

        MouseEnterEvent* event =  BX_NEW(ctx->getAllocator(), MouseEnterEvent);
        event->m_entered = entered;
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed mouse enter event");
    });
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        static GLFWContext *const ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));

        MouseButtonEvent* event =  BX_NEW(ctx->getAllocator(), MouseButtonEvent);
        event->m_button = (MouseButtonEnum)button;
        event->m_action = (ActionEnum)action;
        event->m_mods = (ModsEnum)mods;
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed mouse button event");
    });
    // scroll callback
    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset){
        static GLFWContext *const ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
        
        ScrollEvent* event =  BX_NEW(ctx->getAllocator(), ScrollEvent);
        event->m_delta = { xOffset, yOffset };
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed mouse scroll event");
    });
    // drop paths callback
    glfwSetDropCallback(m_window, [](GLFWwindow* window, int count, const char** paths) {
        static GLFWContext *const ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window)); 

        DropPathEvent* event = BX_NEW(ctx->getAllocator(), DropPathEvent);
        event->m_paths.reserve(count);
        for(int i = 0; i < count; i++) {
            event->m_paths.emplace_back(paths[i]);
        }
        ctx->pushEvent(event);
        BIGG_GLFW_LOG_TRACE("Pushed drop path event");
    });
    // post window size event
    return false;
}
bool GLFWContext::handleWindowDestruction(WindowDestroyEvent* event) {
    BIGG_PROFILE_GLFW_FUNCTION;
    // destroy window
    glfwDestroyWindow(m_window);

    // Rationale: we check if m_window == nullptr in createWindow so must reset it.
    m_window = nullptr;
    return false;
}
bool GLFWContext::handleWindowSize(WindowSizeEvent* event) { 
    if(GLFWContext::getWindowSize() == event->m_size) {
        // window size physically changed. No need to do anything.
        return false;   // didn't handle this event
    }
    // else, window size changed programmatically. Need to resize window.
    glfwSetWindowSize(m_window, event->m_size.x, event->m_size.y);
    return true;    // handled this event.
}
bool GLFWContext::handleWindowPosition(WindowPositionEvent* event) {
    if(GLFWContext::getWindowPosition() == event->m_position) {
        // window position physically changed. No need to do anything.
        return false;   // didn't handle this event
    }
    // else, window size changed programmatically. Need to resize window.
    glfwSetWindowPos(m_window, event->m_position.x, event->m_position.y);
    return true;    // handled this event.
}
bool GLFWContext::handleWindowIconified(WindowIconifyEvent* event) {
    if(GLFWContext::getWindowIconified() == event->m_iconified){
        // window iconified physically. No need to handle anything.
        return false;
    } 
    // else, window iconified programmatically. Need to iconify window.
    if(event->m_iconified) {
        glfwIconifyWindow(m_window);
    } else { 
        glfwRestoreWindow(m_window);
    }
    return true;    // handled this event.
}
bool GLFWContext::handleWindowMaximized(WindowMaximizeEvent* event) {
    if(GLFWContext::getWindowMaximized() == event->m_maximized){
        // window iconified physically. No need to handle anything.
        return false;
    } 
    // else, window iconified programmatically. Need to iconify window.
    if(event->m_maximized) {
        glfwMaximizeWindow(m_window);
    } else { 
        glfwRestoreWindow(m_window);
    }
    return true;    // handled this event.
}
bool GLFWContext::handleWindowFocus(WindowFocusEvent* event) {
    if(GLFWContext::getWindowFocus() == event->m_focused){
        // window iconified physically. No need to handle anything.
        return false;
    } 
    // else, window iconified programmatically. Need to iconify window.
    if(event->m_focused) {
        glfwFocusWindow(m_window);
    } else { 
        // there's no way to un-focus a window
    }
    return true;    // handled this event.
}

// getters
void* GLFWContext::getNativeWindowHandle() {
#   if defined(__APPLE__)
        BIGG_GLFW_LOG_TRACE("Adding metal layer to cocoa window!");
        void* out = glfwGetCocoaWindow(m_window);
        BIGG_ASSERT(out != nullptr, "glfwGetCocoaWindow is nullptr!");
		return addMetalLayerToCocoaWindow(glfwGetCocoaWindow(m_window));
#	elif defined(_WIN32)
		return glfwGetWin32Window(m_window);
#   else
        BIGG_GLFW_LOG_CRITICAL("cannot get native window for non macos or windows platforms!");
        return nullptr;
#	endif // BX_PLATFORM_
}

void* GLFWContext::getNativeDisplayHandle() {
#   if defined(__APPLE__) || defined(_WIN32)
        return nullptr;
#   else
        BIGG_GLFW_LOG_CRITICAL("cannot get native display for non macos or windows platforms!");
        return nullptr;
#   endif
}
glm::ivec2 GLFWContext::getWindowSize() {
    int w = 0, h = 0;
    glfwGetWindowSize(m_window, &w, &h);
    return {w, h};
}
glm::ivec2 GLFWContext::getWindowFramebufferSize() {
    int w = 0, h = 0;
    glfwGetFramebufferSize(m_window, &w, &h);
    return {w, h};
}
glm::vec2 GLFWContext::getWindowContentScale() {
    float x = 0, y = 0;
    glfwGetWindowContentScale(m_window, &x, &y);
    return {x, y};
}
glm::ivec2 GLFWContext::getWindowPosition() {
    int x = 0, y = 0;
    glfwGetWindowPos(m_window, &x, &y);
    return {x, y};
}
bool GLFWContext::getWindowIconified() {
    return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
}
bool GLFWContext::getWindowMaximized() {
    return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
}
bool GLFWContext::getWindowVisible() {
    return glfwGetWindowAttrib(m_window, GLFW_VISIBLE);
}
bool GLFWContext::getWindowFocus() {
    return glfwGetWindowAttrib(m_window, GLFW_FOCUSED);
}

ActionEnum GLFWContext::getKey(KeyEnum key) {
    return (ActionEnum)(glfwGetKey(m_window, (int)key));
}
glm::dvec2 GLFWContext::getMousePosition() {
    double x = 0, y = 0;
    glfwGetCursorPos(m_window, &x, &y);
    return {x, y};
}
bool GLFWContext::getMouseHover() {
    return glfwGetWindowAttrib(m_window, GLFW_HOVERED);
}
ActionEnum GLFWContext::getMouseButton(MouseButtonEnum button) {
    return (ActionEnum)(glfwGetMouseButton(m_window, (int)button));
}
std::string GLFWContext::getClipboardString() {
    return {glfwGetClipboardString(m_window)};
}

// setters

void GLFWContext::setWindowShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(m_window, shouldClose);
}

void GLFWContext::setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) {
    glfwSetWindowSizeLimits(m_window, minWidth, minHeight, maxWidth, maxHeight);
}

void GLFWContext::setWindowAspectRatio(int numerator, int denominator) {
    glfwSetWindowAspectRatio(m_window, numerator, denominator);
}

void GLFWContext::setWindowTitle(std::string title) {
    glfwSetWindowTitle(m_window, title.c_str());
}
void GLFWContext::setWindowVisible(bool visible) {
    if(visible) {
        glfwShowWindow(m_window);
    } else {
        glfwHideWindow(m_window);
    }
}

void GLFWContext::setClipboardString(std::string text) {
    glfwSetClipboardString(NULL, text.c_str());
}

ContextI* createInstance() {
    return new GLFWContext();
}

}  // namespace BIGGEngine