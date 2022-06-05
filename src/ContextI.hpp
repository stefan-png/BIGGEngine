#pragma once

#include <stdint.h>         // for uint32_t
#include <bx/spscqueue.h>   // for m_queue
#include <string>           // for window titles
#include <map>              // for EventCallbacks

#include "Events.hpp"

namespace BIGGEngine {

/** @struct Base class for a context.
 *  A Context is an abstraction over the window, input, and event systems.
 */
struct ContextI {
    
    // These constructors assume m_allocator is initialized first, 
    // which should be true as long as m_allocator is declared first 
    ContextI();
    ContextI(bx::AllocatorI* allocator);
    virtual ~ContextI();

private:
    bx::AllocatorI* getDefaultAllocator();

protected:
    void postUpdateEvent(double delta);
    void postTickEvent(double delta);

public:
    void postWindowCreateEvent(glm::ivec2 size, const char* title);
    void postWindowDestroyEvent();
    void postWindowSizeEvent(glm::ivec2 size);
    void postWindowPositionEvent(glm::ivec2 position);
    void postWindowIconifyEvent(bool iconify);
    void postWindowMaximizeEvent(bool maximize);
    void postWindowFocusEvent(bool focus);

    /** 
     * Main loop to be overridden by the implementation 
     * @warning Blocks the calling thread!
    */
    virtual int run() = 0;

    /**
     * @defgroup VirtualGettersAndSetters Virtual getters and setters. @{
     */
    virtual void* getNativeWindowHandle() = 0;
    virtual void* getNativeDisplayHandle() = 0;

    virtual glm::ivec2 getWindowSize() = 0;
    virtual glm::ivec2 getWindowFramebufferSize() = 0;
    virtual glm::vec2 getWindowContentScale() = 0;
    virtual glm::ivec2 getWindowPosition() = 0;
    virtual bool getWindowIconified() = 0;
    virtual bool getWindowMaximized() = 0;
    virtual bool getWindowVisible() = 0;
    virtual bool getWindowFocus() = 0;
    virtual ActionEnum getKey(KeyEnum key) = 0;
    virtual glm::dvec2 getMousePosition() = 0;
    virtual bool getMouseHover() = 0;
    virtual ActionEnum getMouseButton(MouseButtonEnum button) = 0;
    virtual std::string getClipboardString() = 0;

    virtual void setWindowShouldClose(bool shouldClose) = 0;
    virtual void setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) = 0;
    virtual void setWindowAspectRatio(int numerator, int denominator) = 0;
    virtual void setWindowTitle(std::string title) = 0;
    virtual void setWindowVisible(bool visible) = 0;
    virtual void setClipboardString(std::string text) = 0;
    /** @} */   // end VirtualGettersAndSetters group

    // TODO set window icon
    // TODO get/set monitor???

protected:

    /**
     * @defgroup VirtualHandlers Virtual handlers. @{
     */
    virtual bool handleWindowCreation(WindowCreateEvent* event) = 0;
    virtual bool handleWindowDestruction(WindowDestroyEvent* event) = 0;
    virtual bool handleWindowSize(WindowSizeEvent* event) = 0;
    virtual bool handleWindowPosition(WindowPositionEvent* event) = 0;
    virtual bool handleWindowIconified(WindowIconifyEvent* event) = 0;
    virtual bool handleWindowMaximized(WindowMaximizeEvent* event) = 0;
    virtual bool handleWindowFocus(WindowFocusEvent* event) = 0;
    /** @} */   // end VirtualHandlers group
public:

    /**
     * Pass a callback to be notified when events occur.
     * @param priority @c int8_t Priority for this function in the callback queue. Lower priorities are called first. <b>Negative priorities are reserved for internal use only!</b>
     * @param func @ref @c EVENT_CB_TYPE callback function.
     */
    bool subscribe(int8_t priority, EVENT_CB_TYPE func);

    ///For each event, loops through each callback and invokes it until its handled
    void pollEvents();

    /// Pushes one event to the end of the queue.
    void pushEvent(Events* event);

    /// Returns the context's allocator
    bx::AllocatorI* getAllocator();

    static ContextI* getInstance();
    static void shutdown();

private:
    // Events State
    // Decleration order is important! m_allocator must be declared first, because of constructor.
    // in definition bx/inline/spscunboundedqueue.inl the template is automatically changed to T* (so Events* in this case).
    bx::AllocatorI* m_allocator;
    bx::SpScUnboundedQueueT<Events> m_queue;
    std::map<int8_t, EVENT_CB_TYPE> m_callbacks;

    static inline ContextI* m_instance = nullptr;
};

    /// must call return a pointer to derived ContextI object constructed with 'new'
    /// eg. return new GLFWContext();
    extern ContextI* createInstance();
}   //namespace BIGGEngine