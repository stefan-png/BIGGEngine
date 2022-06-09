#pragma once

#include "Debug.hpp"
#include "InputEnums.hpp"
#include "Functor.hpp"

#include <functional>
#include <glm/vec2.hpp>

#include <string>
#include <vector>
#include <map>

#define EVENT_CB_TYPE std::function<bool(Events*)>
//#define EVENT_CB_TYPE EventFunctor

#define SETUP_EVENT(_eventType) _eventType ## Event() {} \
    Events::EventType m_type = Events::EventType::_eventType;

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

    struct Events {
        /// Events Handling functions
    public:
        // define all EventTypes as an enum struct
        enum struct EventType {
#           define PUT_COMMA(T) T,
            FOR_EACH_EVENT(PUT_COMMA)
        };

        template<typename Event>
        static bool subscribe(uint16_t priority, EventFunctor<Event> functor) {
            if (m_callbacks<Event>.count()) {
                return false;
            }
            m_callbacks<Event>[priority] = functor;
            return true;
        }

        template<typename Event>
        static bool unsubscribe(uint16_t priority) {
            return m_callbacks<Event>.erase(priority);;
        }

        template<typename Event>
        static void setEvent(Event&& event) {
            m_event<Event>(std::forward<Event>(event));
            m_handled < Event > = false;
        }


        template<typename Event>
        static void pollEvent() {
            for(auto& [priority, functor] : m_callbacks<Event>) {
                m_handled<Event> = functor.operator()(m_event<Event>);
                if(m_handled<Event>) break;
            }
        }

        // defined later since it depends on UpdateEvent, ..., xxxEvent
        static void pollEvents();
        // reset Events system. Unsubscribe all callbacks, reset all events.
        static void reset();

    private:

        template<typename Event>
        static std::map<uint16_t, EventFunctor<Event>> m_callbacks;

        template<typename Event>
        static Event m_event;

        template<typename Event>
        static bool m_handled;

    public:
        Events() = delete;
    };

    // Runtime Events
    struct UpdateEvent {
        SETUP_EVENT(Update)

        double m_delta;
    };

    struct TickEvent {
        SETUP_EVENT(Tick)

        double m_delta;
    };


// Window Events
    struct WindowCreateEvent {
        SETUP_EVENT(WindowCreate)

        glm::ivec2 m_size;
        std::string m_title;
    };
    struct WindowDestroyEvent {
        SETUP_EVENT(WindowDestroy)
    };
    struct WindowShouldCloseEvent {
        SETUP_EVENT(WindowShouldClose)
    };
    struct WindowSizeEvent {
        SETUP_EVENT(WindowSize)

        glm::ivec2 m_size;
    };
    struct WindowFramebufferSizeEvent {
        SETUP_EVENT(WindowFramebufferSize)

        glm::ivec2 m_size;
    };
    struct WindowContentScaleEvent {
        SETUP_EVENT(WindowContentScale)

        glm::vec2 m_scale;
    };
    struct WindowPositionEvent {
        SETUP_EVENT(WindowPosition)

        glm::ivec2 m_position;
    };
    struct WindowIconifyEvent {
        SETUP_EVENT(WindowIconify)

        bool m_iconified;
    };
    struct WindowMaximizeEvent {
        SETUP_EVENT(WindowMaximize)

        bool m_maximized;
    };
    struct WindowFocusEvent {
        SETUP_EVENT(WindowFocus)

        bool m_focused;
    };
    struct WindowRefreshEvent {
        SETUP_EVENT(WindowRefresh)
    };

    struct KeyEvent {
        SETUP_EVENT(Key)

        KeyEnum m_key;
        int m_scancode;
        ActionEnum m_action;
        ModsEnum m_mods;
    };
    struct CharEvent {
        SETUP_EVENT(Char)

        unsigned int m_codepoint;
    };

    struct MousePositionEvent {
        SETUP_EVENT(MousePosition)

        glm::dvec2 m_mousePosition;
        glm::dvec2 m_delta;
    };
    struct MouseEnterEvent {
        SETUP_EVENT(MouseEnter)

        bool m_entered;
    };
    struct MouseButtonEvent {
        SETUP_EVENT(MouseButton)

        MouseButtonEnum m_button;
        ActionEnum m_action;
        ModsEnum m_mods;
    };

    struct ScrollEvent {
        SETUP_EVENT(Scroll)

        glm::dvec2 m_delta;
    };

    struct DropPathEvent {
        SETUP_EVENT(DropPath)

        std::vector<std::string> m_paths;
    };

    // define Events::pollEvents() after defining all event types.
    void Events::pollEvents() {
#       define POLL_EVENT(T) pollEvent<T ## Event>();
        FOR_EACH_EVENT(POLL_EVENT)
    }
    void Events::reset() {
#       define UNSUBSCRIBE_EVENT(T) m_callbacks<T ## Event>.clear();
#       define SET_HANDLED(T, value) m_handled<T ## Event> = value;
        FOR_EACH_EVENT(UNSUBSCRIBE_EVENT)
        FOR_EACH_EVENT(SET_HANDLED, false)
    }

};  // namespace BIGGEngine