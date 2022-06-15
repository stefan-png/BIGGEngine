#include "RenderBase.hpp"
#include "../Core.hpp"
#include "../Context.hpp"
#include <glm/vec2.hpp>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define BIGG_PROFILE_RENDERER_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("renderer")
#define BIGG_PROFILE_RENDERER_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("renderer", _format, ##__VA_ARGS__)

namespace BIGGEngine {
namespace RenderBase {
namespace {

    uint32_t resetFlags = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X16;

    bool handleWindowCreateEvent(WindowCreateEvent){
        BIGG_PROFILE_RENDERER_FUNCTION;

        bgfx::PlatformData pd;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
        pd.ndt = Context::getNativeDisplayHandle();
        pd.nwh = Context::getNativeWindowHandle();

        glm::ivec2 fbSize = Context::getWindowFramebufferSize();

        bgfx::Init init;
        init.platformData = pd;
        init.debug = false;
        init.resolution.width = fbSize.x;
        init.resolution.height = fbSize.y;
        // These things could be determined by bgfx if I just leave them uninitialized
        init.resolution.reset = resetFlags;
//        init.vendorId = BGFX_PCI_ID_APPLE;
//        init.type = bgfx::RendererType::Metal;
        bgfx::init(init);

        // TODO macro/loop for each view
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x939762ff);
        bgfx::setViewRect(0, 0, 0, fbSize.x, fbSize.y);
        bgfx::setViewRect(1, 0, 0, fbSize.x, fbSize.y);
        bgfx::setViewRect(2, 0, 0, fbSize.x, fbSize.y);

        return false;
    }
    bool handleWindowSizeEvent(WindowSizeEvent event) {
        bgfx::reset((uint32_t)event.m_size.x, (uint32_t)event.m_size.y, resetFlags);

        // TODO macro/loop for each view
        bgfx::setViewRect(0, 0, 0, uint16_t(event.m_size.x), uint16_t(event.m_size.y));
        bgfx::setViewRect(1, 0, 0, uint16_t(event.m_size.x), uint16_t(event.m_size.y));
        bgfx::setViewRect(2, 0, 0, uint16_t(event.m_size.x), uint16_t(event.m_size.y));
        return false;
    }
    bool handleLateUpdateEvent(UpdateEvent) {
        BIGG_PROFILE_RENDERER_FUNCTION;
        bgfx::frame();
        return false;
    }
}
    void init() {
        BIGG_PROFILE_INIT_FUNCTION;

        Events::subscribe<WindowCreateEvent>(g_renderBaseBeginPriority, handleWindowCreateEvent);
        Events::subscribe<WindowSizeEvent>(g_renderBaseBeginPriority,   handleWindowSizeEvent);
        Events::subscribe<UpdateEvent>(g_renderBaseEndPriority,         handleLateUpdateEvent);
    }

} // namespace RenderBase
} // namespace BIGGEngine