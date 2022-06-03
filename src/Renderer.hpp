#pragma once

#include "ContextI.hpp"
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