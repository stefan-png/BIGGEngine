#include "../src/Core.hpp"
#include "../src/Context.hpp"
#include "../src/ContextImplGLFW.hpp"
#include "../src/Render/RenderBase.hpp"
#include "../src/Render/RenderUI.hpp"

//#include "../src/Script.hpp"

#include <imgui.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>    // for glm::one_over_root_two

namespace BIGGEngine {

struct App {
    
    App()  {
        BIGG_PROFILE_INIT_FUNCTION;

        Events::setEvent<WindowCreateEvent>({{720, 600}, "Best Window in the World"});
        Events::subscribe<UpdateEvent>(100, [this](UpdateEvent) { return update(); });
        Events::subscribe<MouseEnterEvent>(100, [](MouseEnterEvent e) { BIGG_LOG_INFO("mouse entered: {}", e.m_entered); return false; });
        Events::subscribe<ScrollEvent>(100, [](ScrollEvent e) { BIGG_LOG_INFO("scrolled {: .2f}", e.m_delta); return false; } );

        Context::init();
        GLFWContext::init();
        RenderBase::init();
        RenderUI::init();

        glm::vec2 vec2{0.129387198357, 4.598237452};
        glm::vec3 vec3{0.129387198357, 4.598237452, -3.143322};
        glm::vec4 vec4{0.129387198357, 4.598237452, -9.587232, 0.005125};
        glm::mat4x4 mat4{{0.1, 0.23, 0.4, 0.0},
                         {0.1, 523.3, 0.3, 0.0},
                         {0, -0.00, 0, 0},
                         {1, 35, 2, 3}};
        glm::quat quat{1.0f, glm::one_over_root_two<float>(), 0.0f, 0.0f};

        BIGG_LOG_INFO("my vec2 is {: ^.1f}.", vec2);
        BIGG_LOG_INFO("my vec3 is {: ^.1f}.", vec3);
        BIGG_LOG_INFO("my vec4 is {: ^8.2f}.", vec4);
        BIGG_LOG_INFO("my mat4 is {: ^8.2f}.", mat4);

        BIGG_LOG_INFO("my quat is {:.4f}.", quat);

        // this should add the script "main" to the registry and run the script once.
//        Scripting::registerScript("test2", "../test/test2.lua");
//        Scripting::registerScript("main", "../test/main.lua");
//
//        auto& reg = ECS::get();
//        const auto entity = reg.create();
//        reg.emplace<Mesh>(entity);
//        reg.emplace<Transform>(entity);
//        reg.emplace<ScriptComponent>(entity, "main");
    }

    ~App() {
        RenderUI::shutdown();
        Context::shutdown();
    }

    void tick(double delta) {
        // BIGG_LOG_INFO("tick: {:3.3f}ms", delta * 1e3);
    }

    bool update() {
        BIGG_PROFILE_RUN_FUNCTION;
        ImGui::ShowDemoWindow();
        return false;
    }

    void printDropPath(DropPathEvent* event) {

        BIGG_PROFILE_RUN_FUNCTION;

        for(std::string s : event->m_paths) {
            BIGG_LOG_INFO("Dropped file {:s}", s);
        }
        if(event->m_paths.size() >= 1) {
            Context::setClipboardString(event->m_paths.at(0));
        } 
    }
};

}   // namespace BIGGEngine

int main() {

    using namespace BIGGEngine;
    App* app;
    int result;
    {
        BIGG_PROFILE_INIT_SCOPE("Init");

        Log::setLevel(Log::LogLevel::Info);
        Log::init();

        Events::init();

        app = new App();
    }
    {
        BIGG_PROFILE_RUN_SCOPE("Run");
        result = Context::run();
    }
    {
        BIGG_PROFILE_SHUTDOWN_SCOPE("Shutdown");
        delete app;
    }
    return result;
}