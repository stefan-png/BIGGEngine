#include "Renderer.hpp"
#include "Core.hpp"
#include <glm/vec2.hpp>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define BIGG_PROFILE_RENDERER_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("renderer")
#define BIGG_PROFILE_RENDERER_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("renderer", _format, ##__VA_ARGS__)

#define VIEWID viewID
#define MAX_VIEWID 1
#define FOREACH_VIEW(statement) for(bgfx::ViewId viewID = 0; viewID <= MAX_VIEWID; viewID++) { \
                                    statement                                                  \
                                    }

namespace BIGGEngine {

Renderer::Renderer() {
    BIGG_PROFILE_INIT_FUNCTION;

    ContextI* ctx = ContextI::getInstance();

    ctx->subscribe(g_rendererBeginPriority, [this](Events* event) -> bool {
        switch(event->m_type) {
            case Events::EventType::WindowCreate:
                return handleWindowCreateEvent();
            case Events::EventType::WindowSize:
                return handleWindowSizeEvent(static_cast<WindowSizeEvent*>(event));
            default:
                return false;
        }
    });

    ctx->subscribe(g_rendererEndPriority, [this](Events* event) -> bool {
        if(event->m_type == Events::EventType::Update) {
            return handleLateUpdateEvent();
        }
        return false;
    });
}

Renderer::~Renderer() {}

bool Renderer::handleWindowSizeEvent(WindowSizeEvent* event) {
    bgfx::reset((uint32_t)event->m_size.x, (uint32_t)event->m_size.y, BGFX_RESET_NONE);
    FOREACH_VIEW(
        bgfx::setViewRect(VIEWID, 0, 0, uint16_t(event->m_size.x), uint16_t(event->m_size.y));
    )
    return false;
}

bool Renderer::handleWindowCreateEvent() {
    BIGG_PROFILE_RENDERER_FUNCTION;

    ContextI* ctx = ContextI::getInstance();

    bgfx::PlatformData pd;
    pd.context = nullptr;
    pd.backBuffer = nullptr;
    pd.backBufferDS = nullptr;
    pd.ndt = ctx->getNativeDisplayHandle();
    pd.nwh = ctx->getNativeWindowHandle();

    glm::ivec2 fbSize = ctx->getWindowFramebufferSize();

    bgfx::Init init;
    init.platformData = pd;
    init.debug = false;
    init.resolution.width = fbSize.x;
    init.resolution.height = fbSize.y;
    // These things could be determined by bgfx if I just leave them uninitialized
    // init.resolution.reset = BGFX_RESET_VSYNC;
    init.vendorId = BGFX_PCI_ID_APPLE;
    init.type = bgfx::RendererType::Metal;
    bgfx::init(init);


    FOREACH_VIEW(
            bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x939762ff);
        bgfx::setViewRect(VIEWID, 0, 0, fbSize.x, fbSize.y);
    )

    return false;
}

bool Renderer::handleLateUpdateEvent() {
    BIGG_PROFILE_RENDERER_FUNCTION;
    bgfx::frame();
    return false;
}

}   // namespace BIGGEngine