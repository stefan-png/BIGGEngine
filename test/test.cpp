#include "../src/Core.hpp"
#include "../src/Renderer.hpp"

#include "../src/ContextImplGLFW.hpp"
#include "../src/Render/RenderUI-inl.hpp"
#include "../src/Render/RenderMeshComponents-inl.hpp"

#include "../src/Script.hpp"

#include <imgui.h>

#include <entt/entt.hpp>

#include <bgfx/bgfx.h>

namespace BIGGEngine {

    struct Vertex
    {
        float x;
        float y;
        float z;
        uint32_t abgr;
    };

    const Vertex vertices[] = {
            {-1.0f,  1.0f,  1.0f, 0xff000000 },
            { 1.0f,  1.0f,  1.0f, 0xff0000ff },
            {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
            { 1.0f, -1.0f,  1.0f, 0xff00ffff },
            {-1.0f,  1.0f, -1.0f, 0xffff0000 },
            { 1.0f,  1.0f, -1.0f, 0xffff00ff },
            {-1.0f, -1.0f, -1.0f, 0xffffff00 },
            { 1.0f, -1.0f, -1.0f, 0xffffffff },
    };

    const uint16_t indices[] = {
            0, 1, 2,
            1, 3, 2,
            4, 6, 5,
            5, 6, 7,
            0, 2, 4,
            4, 2, 6,
            1, 5, 3,
            5, 7, 3,
            0, 4, 1,
            4, 5, 1,
            2, 3, 6,
            6, 3, 7,
    };

    static const bgfx::Memory* loadMem(const char* filepath) {
        static bx::DefaultAllocator s_allocator;
        bx::FileReaderI* fileReader = BX_NEW(&s_allocator, bx::FileReader);

        if (bx::open(fileReader, filepath))
        {
            uint32_t size = (uint32_t)bx::getSize(fileReader);
            const bgfx::Memory* mem = bgfx::alloc(size+1);
            bx::read(fileReader, mem->data, size, bx::ErrorAssert{});
            bx::close(fileReader);
            mem->data[mem->size-1] = '\0';
            return mem;
        } else {
            spdlog::critical("Failed to load {}.", filepath);
            system("pwd");
            abort();
        }
    }

struct App {
    
    App() : m_context(), m_renderer(&m_context), m_renderUI(&m_context),  m_renderMeshComponents(&m_context, m_registry) {

        BIGG_PROFILE_INIT_FUNCTION;

        m_context.postWindowCreateEvent({720, 600}, "Best Window in the World");

        m_context.subscribe(1, [this](Event* e) -> bool {
            return handleEvents(e);
        });

        // this should add the script "main" to the registry and run the script once.
        Scripting::registerScript("main", "../test/main.lua");

        const auto entity = m_registry.create();
        m_registry.emplace<Mesh>(entity);
        m_registry.emplace<Transform>(entity);
        m_registry.emplace<ScriptComponent>(entity, "main");
    }

    ~App() {}

    bool handleEvents(Event* event) {

        switch(event->m_type) {
            case Event::EventType::WindowCreate:
                setup();
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
                shutdown();
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

    void setup() {
        vertexLayout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

        vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), vertexLayout);
        indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

        bgfx::ShaderHandle vs = bgfx::createShader(loadMem("../thirdparty/bgfx/examples/runtime/shaders/metal/vs_cubes.bin"));
        bgfx::ShaderHandle fs = bgfx::createShader(loadMem("../thirdparty/bgfx/examples/runtime/shaders/metal/fs_cubes.bin"));
        program = bgfx::createProgram(vs, fs, true);

        bgfx::destroy(vs);
        bgfx::destroy(fs);
    }

    bgfx::VertexLayout vertexLayout;
    bgfx::VertexBufferHandle vertexBuffer;
    bgfx::IndexBufferHandle indexBuffer;
    bgfx::ProgramHandle program;



    void update(double delta) {

        ImGui::ShowDemoWindow();
        bgfx::touch(0);
        return;
    }

    void shutdown() {
        bgfx::destroy(vertexBuffer);
        bgfx::destroy(indexBuffer);
        bgfx::destroy(program);
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

    entt::registry m_registry;
    RenderMeshComponents m_renderMeshComponents;
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