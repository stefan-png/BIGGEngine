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
namespace GLFWContext {
namespace {

    //TODO this should be an array of size MAX_WINDOWS
    GLFWwindow *window = nullptr;

    bool handleWindowCreation(WindowCreateEvent event) {
        BIGG_PROFILE_GLFW_FUNCTION;
        // create glfw window
        if (window != nullptr) {
            BIGG_GLFW_LOG_WARN("Tried to create a window when one already exists!");
            return false;
        }

        window = glfwCreateWindow(event.m_size.x, event.m_size.y, event.m_title.c_str(), nullptr, nullptr);
        if (window == nullptr) {

            BIGG_GLFW_LOG_WARN("Failed to create a window!");
            return false;
        }

        glfwSetWindowCloseCallback(window, [](GLFWwindow *window) {
            Events::setEvent<WindowShouldCloseEvent>(WindowShouldCloseEvent{});
        });
        glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            Events::setEvent<WindowSizeEvent>(WindowSizeEvent{{width, height}});
            Events::pollEvent<WindowSizeEvent>(); // propogate the event immediately
        });
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            Events::setEvent<WindowFramebufferSizeEvent>(WindowFramebufferSizeEvent{{width, height}});
        });
        glfwSetWindowContentScaleCallback(window, [](GLFWwindow *window, float xScale, float yScale) {
            Events::setEvent<WindowContentScaleEvent>(WindowContentScaleEvent{{xScale, yScale}});
        });
        glfwSetWindowPosCallback(window, [](GLFWwindow *window, int x, int y) {
            Events::setEvent<WindowPositionEvent>(WindowPositionEvent{{x, y}});
            Events::pollEvent<WindowPositionEvent>(); // propogate the event immediately
        });
        glfwSetWindowIconifyCallback(window, [](GLFWwindow *window, int iconified) {
            Events::setEvent<WindowIconifyEvent>(WindowIconifyEvent{static_cast<bool>(iconified)});
            Events::pollEvent<WindowIconifyEvent>(); // propogate the event immediately
        });
        glfwSetWindowMaximizeCallback(window, [](GLFWwindow *window, int maximized) {
            Events::setEvent<WindowMaximizeEvent>({static_cast<bool>(maximized)});
            Events::pollEvent<WindowMaximizeEvent>(); // propogate the event immediately
        });
        glfwSetWindowFocusCallback(window, [](GLFWwindow *window, int focus) {
            Events::setEvent<WindowFocusEvent>({static_cast<bool>(focus)});
            Events::pollEvent<WindowFocusEvent>(); // propogate the event immediately
        });
        glfwSetWindowRefreshCallback(window, [](GLFWwindow *window) {
            Events::setEvent<WindowRefreshEvent>({});
        });
        // key callbacks
        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            Events::setEvent<KeyEvent>({
                                               static_cast<KeyEnum>(key), scancode, static_cast<ActionEnum>(action),
                                               static_cast<ModsEnum>(mods)
                                       });
        });
        glfwSetCharCallback(window, [](GLFWwindow *window, unsigned int codepoint) {
            Events::setEvent<CharEvent>({codepoint});
        });
        // mouse callbacks
        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
            static glm::dvec2 lastMousePos;
            glm::dvec2 currentMousePos(x, y);

            Events::setEvent<MousePositionEvent>({
                                                         currentMousePos,
                                                         currentMousePos - lastMousePos
                                                 });

            lastMousePos.x = x;
            lastMousePos.y = y;
        });
        glfwSetCursorEnterCallback(window, [](GLFWwindow *window, int entered) {
            Events::setEvent<MouseEnterEvent>({static_cast<bool>(entered)});
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
            Events::setEvent<MouseButtonEvent>({
                                                       static_cast<MouseButtonEnum>(button),
                                                       static_cast<ActionEnum>(action), static_cast<ModsEnum>(mods)
                                               });
        });
        // scroll callback
        glfwSetScrollCallback(window, [](GLFWwindow *window, double xOffset, double yOffset) {
            Events::setEvent<ScrollEvent>({{xOffset, yOffset}});
        });
        // drop paths callback
        glfwSetDropCallback(window, [](GLFWwindow *window, int count, const char **paths) {
            std::vector<std::string> vec;
            vec.reserve(count);
            for (int i = 0; i < count; i++) {
                vec.emplace_back(paths[i]);
            }
            Events::setEvent<DropPathEvent>({vec});
        });
        // post window size event
        return false;
    }

    bool handleWindowDestruction(WindowDestroyEvent event) {
        BIGG_PROFILE_GLFW_FUNCTION;
        // destroy window
        glfwDestroyWindow(window);

        // Rationale: we check if window == nullptr in createWindow so must reset it.
        window = nullptr;
        return false;
    }

    bool handleWindowSize(WindowSizeEvent event) {
        if (Context::getWindowSize() == event.m_size) {
            // window size physically changed. No need to do anything.
            return false;   // didn't handle this event
        }
        // else, window size changed programmatically. Need to resize window.
        glfwSetWindowSize(window, event.m_size.x, event.m_size.y);
        return true;    // handled this event.
    }

    bool handleWindowPosition(WindowPositionEvent event) {
        if (Context::getWindowPosition() == event.m_position) {
            // window position physically changed. No need to do anything.
            return false;   // didn't handle this event
        }
        // else, window size changed programmatically. Need to resize window.
        glfwSetWindowPos(window, event.m_position.x, event.m_position.y);
        return true;    // handled this event.
    }

    bool handleWindowIconify(WindowIconifyEvent event) {
        if (Context::getWindowIconified() == event.m_iconified) {
            // window iconified physically. No need to handle anything.
            return false;
        }
        // else, window iconified programmatically. Need to iconify window.
        if (event.m_iconified) {
            glfwIconifyWindow(window);
        } else {
            glfwRestoreWindow(window);
        }
        return true;    // handled this event.
    }

    bool handleWindowMaximize(WindowMaximizeEvent event) {
        if (Context::getWindowMaximized() == event.m_maximized) {
            // window iconified physically. No need to handle anything.
            return false;
        }
        // else, window iconified programmatically. Need to iconify window.
        if (event.m_maximized) {
            glfwMaximizeWindow(window);
        } else {
            glfwRestoreWindow(window);
        }
        return true;    // handled this event.
    }

    bool handleWindowFocus(WindowFocusEvent event) {
        if (Context::getWindowFocus() == event.m_focused) {
            // window iconified physically. No need to handle anything.
            return false;
        }
        // else, window iconified programmatically. Need to iconify window.
        if (event.m_focused) {
            glfwFocusWindow(window);
        } else {
            // there's no way to un-focus a window
        }
        return true;    // handled this event.
    }

}   // anonymous namespace


    void init() {
        // Since contextLogger is already setup, and I just want to change its name, use spdlog::logger::clone()
        Log::m_contextLogger = Log::m_contextLogger->clone("GLFW");

        Events::subscribe<WindowCreateEvent>(g_contextPriority, handleWindowCreation);
        Events::subscribe<WindowDestroyEvent>(g_contextPriority, handleWindowDestruction);
        Events::subscribe<WindowSizeEvent>(g_contextPriority, handleWindowSize);
        Events::subscribe<WindowPositionEvent>(g_contextPriority, handleWindowPosition);
        Events::subscribe<WindowIconifyEvent>(g_contextPriority, handleWindowIconify);
        Events::subscribe<WindowMaximizeEvent>(g_contextPriority, handleWindowMaximize);
        Events::subscribe<WindowFocusEvent>(g_contextPriority, handleWindowFocus);
    }
}   // namespace GLFWContext

    int ::BIGGEngine::Context::run() {
        {
            BIGG_PROFILE_GLFW_SCOPE("glfwInit()");
            BIGG_GLFW_LOG_DEBUG("Running GLFWContext...");
            glfwSetErrorCallback([](int error, const char *description) {
                BIGG_GLFW_LOG_CRITICAL(" GLFW Error Code {:d}: {:s}", error, description);
            });

            if (!glfwInit()) {
                BIGG_GLFW_LOG_CRITICAL("GLFW context failed to initialize!");
            }
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   // important so that window doesn't go black as soon as I move it
        // glfwSwapInterval(1);    //enable vsync
        // poll events once to see if need to create/destroy some windows
        Events::pollEvent<WindowCreateEvent>();
        // At this point, the only callback which is subscribed should be
        // the one in ContextI.cpp constructor (ie window create/distroy)
        // Therefore user didn't create a window.
        if (GLFWContext::window == nullptr) {
            BIGG_GLFW_LOG_WARN("The context has no windows! Try submitting a WindowCreateEvent");
        }

        // The way this loop ends up working is that its bottlenecked by how
        // quickly GLFW can poll for events. On my system, mouse events occur
        // every ~8ms or ~11ms with a bluetooth mouse so that is the most rapidly
        // the game will recieve update events. If the mouse isn't moving, the game
        // will recieve an update event every tick, since the world changes at this
        // rate, it makes sense.
        double lastUpdateTime, lastTickTime, timeoutTime, now;
        lastUpdateTime = lastTickTime = now = Profile::now();
        while (GLFWContext::window != nullptr && !glfwWindowShouldClose(GLFWContext::window)) {
            {
                BIGG_PROFILE_GLFW_SCOPE("glfwWaitEventsTimeout({:.3f})", g_tickDeltaTime);
                // poll glfw events (which will populate context event queue)
                glfwWaitEventsTimeout(g_tickDeltaTime);    // at least once per tick
            }

            now = Profile::now();

            // post an update event
//        postUpdateEvent(now - lastUpdateTime);
            Events::setEvent<UpdateEvent>(UpdateEvent{now - lastUpdateTime});
            lastUpdateTime = now;

                // post a tick event
            if (now - lastTickTime >= g_tickDeltaTime) {
//            postTickEvent(now - lastTickTime);
                Events::setEvent<TickEvent>(TickEvent{now - lastTickTime});
                lastTickTime = now;
            }

                // poll context events
            Events::pollEvents();
        }
        BIGG_GLFW_LOG_DEBUG("Terminating GLFWContext...");

        glfwTerminate();
        return EXIT_SUCCESS;   // return from main
    }


// getters
        void *Context::getNativeWindowHandle() {
#   if defined(__APPLE__)
            BIGG_GLFW_LOG_TRACE("Adding metal layer to cocoa window!");
            void *out = glfwGetCocoaWindow(GLFWContext::window);
            BIGG_ASSERT(out != nullptr, "glfwGetCocoaWindow is nullptr!");
            return addMetalLayerToCocoaWindow(glfwGetCocoaWindow(GLFWContext::window));
#	elif defined(_WIN32)
            return glfwGetWin32Window(GLFWContext::window);
#   else
            BIGG_GLFW_LOG_CRITICAL("cannot get native window for non macos or windows platforms!");
            return nullptr;
#	endif // BX_PLATFORM_
        }

        void *Context::getNativeDisplayHandle() {
#   if defined(__APPLE__) || defined(_WIN32)
            return nullptr;
#   else
            BIGG_GLFW_LOG_CRITICAL("cannot get native display for non macos or windows platforms!");
            return nullptr;
#   endif
        }

        glm::ivec2 Context::getWindowSize() {
            int w = 0, h = 0;
            glfwGetWindowSize(GLFWContext::window, &w, &h);
            return {w, h};
        }

        glm::ivec2 Context::getWindowFramebufferSize() {
            int w = 0, h = 0;
            glfwGetFramebufferSize(GLFWContext::window, &w, &h);
            return {w, h};
        }

        glm::vec2 Context::getWindowContentScale() {
            float x = 0, y = 0;
            glfwGetWindowContentScale(GLFWContext::window, &x, &y);
            return {x, y};
        }

        glm::ivec2 Context::getWindowPosition() {
            int x = 0, y = 0;
            glfwGetWindowPos(GLFWContext::window, &x, &y);
            return {x, y};
        }

        bool Context::getWindowIconified() {
            return glfwGetWindowAttrib(GLFWContext::window, GLFW_ICONIFIED);
        }

        bool Context::getWindowMaximized() {
            return glfwGetWindowAttrib(GLFWContext::window, GLFW_MAXIMIZED);
        }

        bool Context::getWindowVisible() {
            return glfwGetWindowAttrib(GLFWContext::window, GLFW_VISIBLE);
        }

        bool Context::getWindowFocus() {
            return glfwGetWindowAttrib(GLFWContext::window, GLFW_FOCUSED);
        }

        ActionEnum Context::getKey(KeyEnum key) {
            return (ActionEnum)(glfwGetKey(GLFWContext::window, (int) key));
        }

        glm::dvec2 Context::getMousePosition() {
            double x = 0, y = 0;
            glfwGetCursorPos(GLFWContext::window, &x, &y);
            return {x, y};
        }

        bool Context::getMouseHover() {
            return glfwGetWindowAttrib(GLFWContext::window, GLFW_HOVERED);
        }

        ActionEnum Context::getMouseButton(MouseButtonEnum button) {
            return (ActionEnum)(glfwGetMouseButton(GLFWContext::window, (int) button));
        }

        std::string Context::getClipboardString() {
            return {glfwGetClipboardString(GLFWContext::window)};
        }

// setters

        void Context::setWindowShouldClose(bool shouldClose) {
            glfwSetWindowShouldClose(GLFWContext::window, shouldClose);
        }

        void Context::setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) {
            glfwSetWindowSizeLimits(GLFWContext::window, minWidth, minHeight, maxWidth, maxHeight);
        }

        void Context::setWindowAspectRatio(int numerator, int denominator) {
            glfwSetWindowAspectRatio(GLFWContext::window, numerator, denominator);
        }

        void Context::setWindowTitle(std::string title) {
            glfwSetWindowTitle(GLFWContext::window, title.c_str());
        }

        void Context::setWindowVisible(bool visible) {
            if (visible) {
                glfwShowWindow(GLFWContext::window);
            } else {
                glfwHideWindow(GLFWContext::window);
            }
        }

        void Context::setClipboardString(std::string text) {
            glfwSetClipboardString(NULL, text.c_str());
        }
}  // namespace BIGGEngine