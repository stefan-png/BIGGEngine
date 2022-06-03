#include "../src/Core.hpp"
#include "../src/Renderer.hpp"
#include "../src/Render/RenderUI-inl.hpp"
#include "../src/Render/RenderMeshComponents-inl.hpp"

#include "../src/Script.hpp"

#include <imgui.h>

#include <bgfx/bgfx.h>

namespace BIGGEngine {

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
    
    App() : m_renderer(), m_renderUI(),  m_renderMeshComponents() {

        BIGG_PROFILE_INIT_FUNCTION;

        ContextI* ctx = ContextI::getInstance();
        ctx->postWindowCreateEvent({720, 600}, "Best Window in the World");

        ctx->subscribe(1, [this](Event* e) -> bool {
            return handleEvents(e);
        });

        // this should add the script "main" to the registry and run the script once.
        Scripting::registerScript("test2", "../test/test2.lua");
        Scripting::registerScript("main", "../test/main.lua");

        auto& reg = ECS::get();
        const auto entity = reg.create();
        reg.emplace<Mesh>(entity);
        reg.emplace<Transform>(entity);
        reg.emplace<ScriptComponent>(entity, "main");
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
                return false;
            case Event::EventType::DropPath:
                printDropPath(static_cast<DropPathEvent*>(event));
                return false;
            case Event::EventType::WindowShouldClose:
                return false;
            default:
                return false;
        }
    }

    void tick(double delta) {
        // BIGG_LOG_INFO("tick: {:3.3f}ms", delta * 1e3);
    }

    void update(double delta) {

        ImGui::ShowDemoWindow();
        bgfx::touch(0);
    }

    void printDropPath(DropPathEvent* event) {

        BIGG_PROFILE_RUN_FUNCTION;

        for(std::string s : event->m_paths) {
            BIGG_LOG_INFO("Dropped file {:s}", s);
        }
        if(event->m_paths.size() >= 1) {
            ContextI::getInstance()->setClipboardString(event->m_paths.at(0));
        } 
    }

    // order here is important because order in initializer list depends on m_context being initialized first!
    Renderer m_renderer;
    RenderUI m_renderUI;
    RenderMeshComponents m_renderMeshComponents;
};

}   // namespace BIGGEngine

int main() {

    using namespace BIGGEngine;
    App* app;
    int result;
    {
        BIGG_PROFILE_INIT_SCOPE("Init");

        Log::setLevel(Log::LogLevel::Debug);
        Log::init();

        ContextI::getInstance();    // call once to initialize ContextI.

        app = new App();

    }
    {
        BIGG_PROFILE_RUN_SCOPE("Run");
        result = ContextI::getInstance()->run();
    }
    {
        BIGG_PROFILE_SHUTDOWN_SCOPE("Shutdown");
        delete app;
    }
    return result;
}