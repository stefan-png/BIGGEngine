#pragma once

#include "ContextI.hpp"
#include "Events.hpp"

namespace BIGGEngine {

struct Renderer {

    Renderer(ContextI* context);
    virtual ~Renderer();

private:
    bool handleWindowCreateEvent();
    bool handleWindowSizeEvent(WindowSizeEvent* event);
    bool handleLateUpdateEvent();
    
    ContextI* m_context;
};

} // namespace BIGGEngine