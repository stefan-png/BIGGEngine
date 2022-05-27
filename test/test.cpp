#include "../src/Core.hpp"
#include "../src/Renderer.hpp"

#include "../src/ContextImplGLFW.hpp"
#include "../src/Render/RenderUI-inl.hpp"

#include "../src/Script.hpp"

#include <imgui.h>

namespace BIGGEngine {

struct App {
    
    App() : m_context(), m_renderer(&m_context), m_renderUI(&m_context) {

        BIGG_PROFILE_INIT_FUNCTION;

        m_context.postWindowCreateEvent({720, 600}, "Best Window in the World");

        m_context.subscribe(1, [this](Event* e) -> bool {
            return handleEvents(e);
        });

        // create test lua thing
        ScriptFunctor scriptFunctor(luaL_newstate(), "../test/main.lua");
        m_context.subscribe(-15, scriptFunctor);
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
            case Event::EventType::MouseButton:
            {
                MouseButtonEvent* e = static_cast<MouseButtonEvent*>(event);
                int mb = static_cast<int>(e->m_button);
                int act = static_cast<int>(e->m_action);
                int mods = static_cast<int>(e->m_mods);
                BIGG_LOG_INFO("TEST mb: {} act: {} mods {:#06b}.", mb, act, mods);
            }
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

#include <lua.hpp>

int doLua() {

    lua_State* L =luaL_newstate();
    luaopen_base(L);

    lua_WarnFunction myWarnFunction = [](void* ud, const char* msg, int tocont){
        BIGG_LOG_WARN("{}", msg);
    };

    lua_setwarnf(L, myWarnFunction, nullptr);

    int result = luaL_dofile(L, "../scripts/main.lua");

    if(result != LUA_OK) {
        const char* errorString = lua_tostring(L, -1);
        BIGG_LOG_CRITICAL("{}", errorString);
        return 1;
    }

    int type = lua_getglobal(L, "B");
    BIGG_LOG_INFO("type of B is {}", type);
    if(type == LUA_TNIL) {
        // in luaconf.h lua_Number is float.
        BIGG_LOG_WARN("this var is nil");
        return 1;
    }
    if(!lua_isnumber(L, -1)) {
        // in luaconf.h lua_Number is float.
        BIGG_LOG_WARN("top of stack is not a number!");
        return 1;
    }


    float a = lua_tonumber(L, -1);
    BIGG_LOG_INFO("Number a is {}", a);
    return 0;
}

int main() {

    using namespace BIGGEngine;
    App* app;
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