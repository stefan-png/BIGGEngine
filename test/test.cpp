#include "../src/Core.hpp"
#include "../src/Context.hpp"
#include "../src/ContextImplGLFW.hpp"
#include "../src/Render/RenderBase.hpp"
#include "../src/Render/RenderUI.hpp"

#include "../src/Script.hpp"

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


        // this should add the script "main" to the registry and run the script once.
        Scripting::registerScript(entt::hashed_string{"test2"}, "../test/test2.lua", 101);
        Scripting::registerScript(entt::hashed_string{"main"}, "../test/main.lua", 102);

        auto& reg = ECS::get();
        const auto entity = reg.create();
        reg.emplace<Mesh>(entity);
        reg.emplace<Transform>(entity);
        reg.emplace<ScriptComponent>(entity, entt::hashed_string{"main"});

        const auto entity2 = reg.create();
        reg.emplace<ScriptComponent>(entity2, entt::hashed_string{"test2"});

        const auto entity3 = reg.create();
        reg.emplace<ScriptComponent>(entity3, entt::hashed_string{"test2"});
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