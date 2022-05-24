#include "../src/Core.hpp"
#include "../src/Renderer.hpp"

#include "../src/ContextImplGLFW.hpp"
#include "../src/Render/RenderUI-inl.hpp"

#include <imgui.h>

namespace BIGGEngine {

struct App {
    
    App() : m_context(), m_renderer(&m_context), m_renderUI(&m_context) {

        BIGG_PROFILE_INIT_FUNCTION;

        m_context.postWindowCreateEvent({720, 600}, "Best Window in the World");

        m_context.subscribe(1, [this](Event* e) -> bool {
            return handleEvents(e);
        });
    }

    ~App() {}

    bool handleEvents(Event* event) {

        switch(event->m_type) {
            case Event::EventType::WindowCreate:
                
                return false;
            case Event::EventType::Update:
                update(static_cast<UpdateEvent*>(event)->m_delta);
                return false;
            case Event::EventType::Tick:
                tick(static_cast<TickEvent*>(event)->m_delta);
                return true;
            case Event::EventType::DropPath:
                printDropPath(static_cast<DropPathEvent*>(event));
                return true;
            case Event::EventType::WindowShouldClose:
                BIGG_LOG_INFO("closing window!");
                return true;
            default:
                return false;
        }
    }

    void tick(double delta) {
        // BIGG_LOG_INFO("tick: {:3.3f}ms", delta * 1e3);
    }

    void update(double delta) {
        // BIGG_LOG_INFO("updt: {:3.3f}ms", delta * 1e3);

        ImGui::ShowDemoWindow();
    }

    void printDropPath(DropPathEvent* event) {

        BIGG_PROFILE_RUN_FUNCTION;

        for(std::string s : event->m_paths) {
            BIGG_LOG_INFO("Dropped file {:s}", s);
        }
        if(event->m_paths.size() >= 1) {
            m_context.setClipboardString(event->m_paths.at(0));
        } 
    }

    // order here is important because order in initializer list depends on m_context being initialized first!
    GLFWContext m_context;
    Renderer m_renderer;
    RenderUI m_renderUI;
};

}   // namespace BIGGEngine

int main() {

    using namespace BIGGEngine;
    App* app;
    KeyEnum someKey = KeyEnum::Zero;

    int result;
    {
        BIGG_PROFILE_INIT_SCOPE("Init");

        Log::setLevel(Log::LogLevel::Info);
        Log::init();
        app = new App();

    }
    {
        BIGG_PROFILE_RUN_SCOPE("Run");
        result = app->m_context.run();
    }
    {
        BIGG_PROFILE_SHUTDOWN_SCOPE("Shutdown");  
        delete app;
    }
    return result;
}