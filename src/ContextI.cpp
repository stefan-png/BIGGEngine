#include "Core.hpp"

#include "ContextI.hpp"

namespace BIGGEngine {

    ContextI::ContextI() : m_allocator(getDefaultAllocator()), m_queue(m_allocator) {
        BIGG_PROFILE_INIT_FUNCTION;
        subscribe(g_contextPriority, [this](Event* event) -> bool{
            // setup callback for events which the context must listen for
            switch(event->m_type) {
                case Event::EventType::WindowCreate:
                    return handleWindowCreation(static_cast<WindowCreateEvent*>(event));
                case Event::EventType::WindowDestroy:
                    return handleWindowDestruction(static_cast<WindowDestroyEvent*>(event));
                case Event::EventType::WindowSize:
                    return handleWindowSize(static_cast<WindowSizeEvent*>(event));
                case Event::EventType::WindowPosition:
                    return handleWindowPosition(static_cast<WindowPositionEvent*>(event));
                case Event::EventType::WindowIconify:
                    return handleWindowIconified(static_cast<WindowIconifyEvent*>(event));
                case Event::EventType::WindowMaximize:
                    return handleWindowMaximized(static_cast<WindowMaximizeEvent*>(event));
                case Event::EventType::WindowFocus:
                    return handleWindowFocus(static_cast<WindowFocusEvent*>(event));
                default:
                    break;
            }
            return false;
        });
    }
    ContextI::ContextI(bx::AllocatorI* allocator) : m_allocator(allocator), m_queue(m_allocator) {}
    ContextI::~ContextI() {    /* TODO should free whole m_queue */ }   

    bx::AllocatorI* ContextI::getDefaultAllocator() {
        static bx::DefaultAllocator defaultAllocator;
        return &defaultAllocator;
    }

    void ContextI::postUpdateEvent(double delta) {
        UpdateEvent* event = BX_NEW(m_allocator, UpdateEvent);
        event->m_delta = delta;
        m_queue.push(event);
    }
    void ContextI::postTickEvent(double delta) {
        TickEvent* event = BX_NEW(m_allocator, TickEvent);
        event->m_delta = delta;
        m_queue.push(event);
    }

    void ContextI::postWindowCreateEvent(glm::ivec2 size, const char* title) {
        WindowCreateEvent* event =  BX_NEW(m_allocator, WindowCreateEvent);
        event->m_size = size;
        event->m_title = title;
        m_queue.push(event);
    }
    void ContextI::postWindowDestroyEvent() {
        WindowDestroyEvent* event = BX_NEW(m_allocator, WindowDestroyEvent);
        m_queue.push(event);
    }
    void ContextI::postWindowSizeEvent(glm::ivec2 size) {
        WindowSizeEvent* event = BX_NEW(m_allocator, WindowSizeEvent);
        event->m_size = size;
        m_queue.push(event);
    }
    void ContextI::postWindowPositionEvent(glm::ivec2 position) {
        WindowPositionEvent* event = BX_NEW(m_allocator, WindowPositionEvent);
        event->m_position = position;
        m_queue.push(event);
    }
    void ContextI::postWindowIconifyEvent(bool iconify) {
        WindowIconifyEvent* event = BX_NEW(m_allocator, WindowIconifyEvent);
        event->m_iconified = iconify;
        m_queue.push(event);
    }
    void ContextI::postWindowMaximizeEvent(bool maximize) {
        WindowMaximizeEvent* event = BX_NEW(m_allocator, WindowMaximizeEvent);
        event->m_maximized = maximize;
        m_queue.push(event);
    }
    void ContextI::postWindowFocusEvent(bool focus) {
        WindowFocusEvent* event = BX_NEW(m_allocator, WindowFocusEvent);
        event->m_focused = focus;
        m_queue.push(event);
    }

    bool ContextI::subscribe(int8_t priority, EVENT_CB_TYPE func) {
        
        if(m_callbacks.count(priority)) {
            // std::map::count returns 1 if key already exists, or 0 if its empty.
            BIGG_LOG_INFO("couldn't subscribed at priority = {:d}", priority);
            return false;
        }
        m_callbacks[priority] = func;
        BIGG_LOG_INFO("subscribed at priority = {:d}", priority);
        return true;
    }

    void ContextI::pollEvents() {
        BIGG_PROFILE_RUN_FUNCTION;
        while(Event* event = m_queue.pop()) {
            {
                BIGG_PROFILE_RUN_SCOPE("Event Type: {}", Debug::g_eventTypeDebugStrings.at(event->m_type));
                for(auto const& [priority, callback] : m_callbacks) {
                    // loop through each callback until one handles the event (returns true)

                    if(callback(event)) {
                        BIGG_LOG_TRACE("polling at priority {:d} handled {:s} event", priority, Debug::g_eventTypeDebugStrings.at(event->m_type));
                        break;
                    }
                    BIGG_LOG_TRACE("polling at priority {:d} didn't handle the {:s} event", priority, Debug::g_eventTypeDebugStrings.at(event->m_type));
                }
            }
            BX_DELETE(m_allocator, event);
        }
    }


    void ContextI::pushEvent(Event* event) {
        m_queue.push(event);
    }

    bx::AllocatorI* ContextI::getAllocator() {
        return m_allocator;
    }

    ContextI* ContextI::getInstance() {
        if(m_instance == nullptr) {
            m_instance = createInstance();
        }
        return m_instance;
    }

    void ContextI::shutdown() {
        delete m_instance;
    }
    

}   // namespace BIGGEngine
