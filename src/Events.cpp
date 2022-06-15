#include "Events.hpp"

#include "Log.hpp"
#include "Debug.hpp"

namespace BIGGEngine {
namespace Events {
namespace {

    template<typename Event>
    std::map<uint16_t, functor_t<Event>> callbacks;

    template<typename Event>
    Event event;

    template<typename Event>
    bool handled;

} // anonymous namespace

    void init() {
#       define INIT_CALLBACK(T) callbacks<T ## Event> = {};
#       define INIT_EVENT(T) event<T ## Event> = {};
#       define SET_HANDLED(T, value) handled<T ## Event> = value;
        FOR_EACH_EVENT(INIT_CALLBACK)
        FOR_EACH_EVENT(INIT_EVENT)
        FOR_EACH_EVENT(SET_HANDLED, true)
    }

    template<typename Event>
    bool subscribe(uint16_t priority, functor_t<Event>&& functor) {
        if (callbacks<Event>.count(priority)) {
            return false;
        }
        callbacks<Event>[priority] = std::forward<functor_t<Event>>(functor);
        return true;
    }

    template<typename Event>
    bool unsubscribe(uint16_t priority) {
        return callbacks<Event>.erase(priority);;
    }

    template<typename Event>
    void setEvent(Event&& e) {
        event<Event> = e;
        handled<Event> = false;
    }

    template<typename Event>
    void pollEvent() {
        if(handled<Event>) return;
        for(auto& [priority, functor] : callbacks<Event>) {
            handled<Event> = functor.operator()(event<Event>);
            if(handled<Event>) break;
        }
        handled<Event> = true;
    }

    void pollEvents() {
#       define POLL_EVENT(T) pollEvent<T ## Event>();
        FOR_EACH_EVENT(POLL_EVENT)
    }
    void reset() {
#       define UNSUBSCRIBE_EVENT(T) callbacks<T ## Event>.clear();
#       define SET_HANDLED(T, value) handled<T ## Event> = value;
        FOR_EACH_EVENT(UNSUBSCRIBE_EVENT)
        FOR_EACH_EVENT(SET_HANDLED, false)
    }

    // explicitly state which template specializations should be created.
#   define EXPLICIT_TEMPLATE_SUBSCRIBE(T) template bool subscribe<T ## Event>(uint16_t, functor_t<T ## Event>&&);
#   define EXPLICIT_TEMPLATE_UNSUBSCRIBE(T) template bool unsubscribe<T ## Event>(uint16_t);
#   define EXPLICIT_TEMPLATE_SET_EVENT(T) template void setEvent<T ## Event>(T ## Event&&);
    FOR_EACH_EVENT(EXPLICIT_TEMPLATE_SUBSCRIBE)
    FOR_EACH_EVENT(EXPLICIT_TEMPLATE_UNSUBSCRIBE)
    FOR_EACH_EVENT(EXPLICIT_TEMPLATE_SET_EVENT)

} // namespace Events
} // namespace BIGGEngine
