#include "../src/Core.hpp"
#include "../src/Context.hpp"
#include "../src/ContextImplGLFW.hpp"
#include "../src/Render/RenderBase.hpp"
#include "../src/Render/RenderMeshComponents.hpp"
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
        Events::subscribe<WindowDestroyEvent>(100, [](WindowDestroyEvent e) { BIGG_LOG_INFO("Window destroyed."); return false; });
        Events::subscribe<ScrollEvent>(100, [](ScrollEvent e) { BIGG_LOG_INFO("scrolled {: .2f}", e.m_delta); return false; } );

        Context::init();
        GLFWContext::init();
        RenderBase::init();
        RenderUI::init();
        RenderMeshComponents::init();


        // this should add the script "main" to the registry and run the script once.
        Scripting::registerScript(entt::hashed_string{"test2"}, "../test/test2.lua", 101);
        Scripting::registerScript(entt::hashed_string{"main"}, "../test/main.lua", 102);

        // TODO instead of registering script, ScriptComponent's constructor should do that
        // automatically.

        auto& reg = ECS::get();
        const auto entity = reg.create();
        reg.emplace<Mesh>(entity);
        reg.emplace<Transform>(entity, glm::vec3{1, 0.0f, 0.0f}, glm::vec3{0, 0, 0}, glm::vec3{2, 0.5, 0.5});
        reg.emplace<ScriptComponent>(entity, entt::hashed_string{"main"});

        const auto entity2 = reg.create();
        reg.emplace<Mesh>(entity2);
        reg.emplace<Transform>(entity2, glm::vec3{-1, 1, 0.0f}, glm::vec3{0, 1, 0}, glm::vec3{0.1, 0.5, 0.5});
        reg.emplace<ScriptComponent>(entity2, entt::hashed_string{"test2"});

        const auto entity3 = reg.create();
        reg.emplace<ScriptComponent>(entity3, entt::hashed_string{"test2"});
        reg.emplace<Mesh>(entity3);
        reg.emplace<Transform>(entity3, glm::vec3{-1, -1.5, 0.0f}, glm::vec3{0, 1, 0}, glm::vec3{0.5, 1, 0.5});

        // TODO make mesh rendering gooder
        // v1 static loading
        // global registry of known meshes
        // - index-able by asset name or path or UUID
        // - metadata:
        //     - bgfx::vertexLayout
        //     - pointer to vertex / index data in heap
        //     - size?
        // allocator with some heap size

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