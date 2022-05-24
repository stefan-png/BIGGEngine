#pragma once

#include "InputEnums.hpp"

#include <functional>
#include <glm/vec2.hpp>

#include <string>
#include <vector>

/**
 * @def EVENT_CB_TYPE std::function<bool(Event*)>
 * A event callback has type @c std::function<bool(Event*)> @c.
 */
#define EVENT_CB_TYPE std::function<bool(Event*)>

/** 
 * @internal
 * @def DEFINE_EVENT_CONSTRUCTOR(_eventType) _eventType ## Event() : Event(Event::EventType::_eventType) {}
 * Utility macro to help define many Event subclasses. 
 */
#define DEFINE_EVENT_CONSTRUCTOR(_eventType) _eventType ## Event() : Event(Event::EventType::_eventType) {}

namespace BIGGEngine {

/**
 * @struct Polymorphic Event type.  
 */
struct Event {

    /**
     * @enum The type of event 
     * 
     */
    enum struct EventType {

        Update,
        Tick,

        WindowCreate,
        WindowDestroy,
        WindowShouldClose,
        WindowSize,
        WindowFramebufferSize,
        WindowContentScale,
        WindowPosition,
        WindowIconify,
        WindowMaximize,
        WindowFocus,
        WindowRefresh,

        Key,
        Char,

        MousePosition,
        MouseEnter,
        MouseButton,

        Scroll,

        DropPath

        // TODO joystick/gamepad input
    };

    Event(EventType type) : m_type(type) {}

    EventType m_type;
};

/**
 * @defgroup Events Events extending base class @ref Event
 * @{
 */

// Runtime Events
struct UpdateEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(Update)

    double m_delta;
};

struct TickEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(Tick)

    double m_delta;
};


// Window Events
struct WindowCreateEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowCreate)

    glm::ivec2 m_size;
    std::string m_title;
};
struct WindowDestroyEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowDestroy)
};
struct WindowShouldCloseEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowShouldClose)
};
struct WindowSizeEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowSize)

    glm::ivec2 m_size;
};
struct WindowFramebufferSizeEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowFramebufferSize)

    glm::ivec2 m_size;
};
struct WindowContentScaleEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowContentScale)

    glm::vec2 m_scale;
};
struct WindowPositionEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowPosition)

    glm::ivec2 m_position;
};
struct WindowIconifyEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowIconify)

    bool m_iconified;
};
struct WindowMaximizeEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowMaximize)

    bool m_maximized;
};
struct WindowFocusEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowFocus)

    bool m_focused;
};
struct WindowRefreshEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(WindowRefresh)
};

struct KeyEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(Key)

    KeyEnum m_key;
    int m_scancode;
    ActionEnum m_action;
    ModsEnum m_mods;
};
struct CharEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(Char)

    unsigned int m_codepoint;
};

struct MousePositionEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(MousePosition)

    glm::dvec2 m_mousePosition;
    glm::dvec2 m_delta;
}; 
struct MouseEnterEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(MouseEnter)

    bool m_entered;
};
struct MouseButtonEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(MouseButton)

    MouseButtonEnum m_button;
    ActionEnum m_action;
    ModsEnum m_mods;
};

struct ScrollEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(Scroll)

    glm::dvec2 m_delta;
};

struct DropPathEvent : public Event {
    DEFINE_EVENT_CONSTRUCTOR(DropPath)

    std::vector<std::string> m_paths;
};
/** @} */   // defgroup Events

}   // namespace BIGGEngine