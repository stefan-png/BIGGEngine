#pragma once

#include "Context.hpp"
#include "Events.hpp"

namespace BIGGEngine {

struct Renderer {

    Renderer();
    virtual ~Renderer();

private:
    bool handleWindowCreateEvent();
    bool handleWindowSizeEvent(WindowSizeEvent* event);
    bool handleLateUpdateEvent();
};

} // namespace BIGGEngine