#pragma once

#include "Debug.hpp"
#include "InputEnums.hpp"

//#include "Functor.hpp"
#include <functional>

#include <glm/vec2.hpp>

#include <string>
#include <vector>
#include <map>

template<typename T>
using functor_t = std::function<bool(T)>;

#define ADD_TYPE_MEMBER(_eventType) static inline Events::EventType m_type = Events::EventType::_eventType;

#define FOR_EACH_EVENT(expr, ...) \
    expr(Update, ##__VA_ARGS__)\
    expr(Tick, ##__VA_ARGS__) \
    expr(WindowCreate, ##__VA_ARGS__) \
    expr(WindowDestroy, ##__VA_ARGS__) \
    expr(WindowShouldClose, ##__VA_ARGS__) \
    expr(WindowSize, ##__VA_ARGS__) \
    expr(WindowFramebufferSize, ##__VA_ARGS__) \
    expr(WindowContentScale, ##__VA_ARGS__) \
    expr(WindowPosition, ##__VA_ARGS__) \
    expr(WindowIconify, ##__VA_ARGS__) \
    expr(WindowMaximize, ##__VA_ARGS__) \
    expr(WindowFocus, ##__VA_ARGS__) \
    expr(WindowRefresh, ##__VA_ARGS__) \
    expr(Key, ##__VA_ARGS__) \
    expr(Char, ##__VA_ARGS__) \
    expr(MousePosition, ##__VA_ARGS__) \
    expr(MouseEnter, ##__VA_ARGS__) \
    expr(MouseButton, ##__VA_ARGS__) \
    expr(Scroll, ##__VA_ARGS__) \
    expr(DropPath, ##__VA_ARGS__)

// TODO joystick/gamepad input

namespace BIGGEngine {
namespace Events {

    void init();

    // define all EventTypes as an enum struct
    enum struct EventType {
#           define PUT_COMMA(T) T,
        FOR_EACH_EVENT(PUT_COMMA)
    };

    template<typename Event>
    bool subscribe(uint16_t priority, functor_t<Event> &&functor);

    template<typename Event>
    bool unsubscribe(uint16_t priority);

    template<typename Event>
    void setEvent(Event &&event);


    template<typename Event>
    void pollEvent();

    // defined later since it depends on UpdateEvent, ..., xxxEvent
    void pollEvents();

    // reset Events system. Unsubscribe all callbacks, reset all events.
    void reset();

} // namespace Events

    // Runtime Events
    struct UpdateEvent {
        double m_delta;
        ADD_TYPE_MEMBER(Update)
    };

    struct TickEvent {
        double m_delta;
        ADD_TYPE_MEMBER(Tick)
    };

// Window Events
    struct WindowCreateEvent {
        glm::ivec2 m_size;
        std::string m_title;
        ADD_TYPE_MEMBER(WindowCreate)
    };
    struct WindowDestroyEvent {
        ADD_TYPE_MEMBER(WindowDestroy)
    };
    struct WindowShouldCloseEvent {
        ADD_TYPE_MEMBER(WindowShouldClose)
    };
    struct WindowSizeEvent {
        glm::ivec2 m_size;
        ADD_TYPE_MEMBER(WindowSize)
    };
    struct WindowFramebufferSizeEvent {
        glm::ivec2 m_size;
        ADD_TYPE_MEMBER(WindowFramebufferSize)
    };
    struct WindowContentScaleEvent {
        glm::vec2 m_scale;
        ADD_TYPE_MEMBER(WindowContentScale)
    };
    struct WindowPositionEvent {
        glm::ivec2 m_position;
        ADD_TYPE_MEMBER(WindowPosition)
    };
    struct WindowIconifyEvent {
        bool m_iconified;
        ADD_TYPE_MEMBER(WindowIconify)
    };
    struct WindowMaximizeEvent {
        bool m_maximized;
        ADD_TYPE_MEMBER(WindowMaximize)
    };
    struct WindowFocusEvent {
        bool m_focused;
        ADD_TYPE_MEMBER(WindowFocus)
    };
    struct WindowRefreshEvent {
        ADD_TYPE_MEMBER(WindowRefresh)
    };

    struct KeyEvent {
        KeyEnum m_key;
        int m_scancode;
        ActionEnum m_action;
        ModsEnum m_mods;
        ADD_TYPE_MEMBER(Key)
    };
    struct CharEvent {
        unsigned int m_codepoint;
        ADD_TYPE_MEMBER(Char)
    };

    struct MousePositionEvent {
        glm::dvec2 m_mousePosition;
        glm::dvec2 m_delta;
        ADD_TYPE_MEMBER(MousePosition)
    };
    struct MouseEnterEvent {
        bool m_entered;
        ADD_TYPE_MEMBER(MouseEnter)
    };
    struct MouseButtonEvent {
        MouseButtonEnum m_button;
        ActionEnum m_action;
        ModsEnum m_mods;
        ADD_TYPE_MEMBER(MouseButton)
    };

    struct ScrollEvent {
        glm::dvec2 m_delta;
        ADD_TYPE_MEMBER(Scroll)
    };

    struct DropPathEvent {
        std::vector<std::string> m_paths;
        ADD_TYPE_MEMBER(DropPath)
    };
};  // namespace BIGGEngine