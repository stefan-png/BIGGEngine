#include "Renderer.hpp"
#include "Core.hpp"
#include <glm/vec2.hpp>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define BIGG_PROFILE_RENDERER_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("renderer")
#define BIGG_PROFILE_RENDERER_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("renderer", _format, ##__VA_ARGS__)

namespace BIGGEngine {

Renderer::Renderer(ContextI* context) : m_context(context) {
    BIGG_PROFILE_INIT_FUNCTION;

    m_context->subscribe(g_rendererBeginPriority, [this](Event* event) -> bool {
        switch(event->m_type) {
            case Event::EventType::WindowCreate:
                return handleWindowCreateEvent();
            case Event::EventType::WindowSize:
                return handleWindowSizeEvent(static_cast<WindowSizeEvent*>(event));
            default:
                return false;
        }
    });

    m_context->subscribe(g_rendererEndPriority, [this](Event* event) -> bool {
        if(event->m_type == Event::EventType::Update) {
            return handleLateUpdateEvent();
        }
        return false;
    });
}

Renderer::~Renderer() {}

bool Renderer::handleWindowSizeEvent(WindowSizeEvent* event) {
    bgfx::reset((uint32_t)event->m_size.x, (uint32_t)event->m_size.y, BGFX_RESET_NONE);
    bgfx::setViewRect(0, 0, 0, uint16_t(event->m_size.x), uint16_t(event->m_size.y));
    return false;
}

bool Renderer::handleWindowCreateEvent() {
    BIGG_PROFILE_RENDERER_FUNCTION;

    bgfx::PlatformData pd;
    pd.context = nullptr;
    pd.backBuffer = nullptr;
    pd.backBufferDS = nullptr;
    pd.ndt = m_context->getNativeDisplayHandle();
    pd.nwh = m_context->getNativeWindowHandle();
    // glm::ivec2 frameSize = m_context->getWindowFrameSize()
    bgfx::Init init;
    init.platformData = pd;
    init.debug = false;
    init.resolution.width = 720;
    init.resolution.height = 600;
    // These things could be determined by bgfx if I just leave them uninitialized
    // init.resolution.reset = BGFX_RESET_VSYNC;
    init.vendorId = BGFX_PCI_ID_APPLE;
    init.type = bgfx::RendererType::Metal;
    bgfx::init(init);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x939762ff);
    bgfx::setViewRect(0, 0, 0, 720, 600);
    return false;
}

bool Renderer::handleLateUpdateEvent() {
    BIGG_PROFILE_RENDERER_FUNCTION;
    bgfx::frame();
    return false;
}

}   // namespace BIGGEngine