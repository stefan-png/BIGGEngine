#pragma once
#include "../Core.hpp"
#include <entt/entt.hpp>

#include <bgfx/bgfx.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho()
#include <glm/gtc/type_ptr.hpp>         // for glm::value_ptr() (convert mat4 to float[16])


#define BIGG_PROFILE_RENDERER_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("renderer")
#define BIGG_PROFILE_RENDERER_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("renderer", _format, ##__VA_ARGS__)


namespace BIGGEngine {

    struct RenderMeshComponents {
        RenderMeshComponents(ContextI* context, entt::registry& registry) : m_context(context), m_registry(registry) {
            BIGG_PROFILE_INIT_FUNCTION;
            bool subscribed = m_context->subscribe(g_renderMeshComponentsPriority, [this](Event* event) -> bool{
                switch(event->m_type) {
                    case Event::EventType::WindowCreate:
                        setup();
                        return false;
                    case Event::EventType::Update:
                        update(static_cast<UpdateEvent*>(event));
                        return false;
                    case Event::EventType::WindowShouldClose:
                        shutdown();
                        return false;
                    default:
                        break;
                }
                return false;
            });



            BIGG_ASSERT(subscribed, "priority level {} is not available!", g_renderMeshComponentsPriority);

        }

    private:
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

        void setup() {
            vertexLayout.begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                    .end();

            vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(mesh.m_vertices.data(), sizeof(ImplVertex) * mesh.m_vertices.size()), vertexLayout);
            indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(mesh.m_indices.data(), sizeof(ImplIndex) * mesh.m_indices.size()));

            bgfx::ShaderHandle vs = bgfx::createShader(loadMem("../thirdparty/bgfx/examples/runtime/shaders/metal/vs_cubes.bin"));
            bgfx::ShaderHandle fs = bgfx::createShader(loadMem("../thirdparty/bgfx/examples/runtime/shaders/metal/fs_cubes.bin"));
            program = bgfx::createProgram(vs, fs, true);

            bgfx::destroy(vs);
            bgfx::destroy(fs);
        }

        Mesh mesh;
        bgfx::VertexLayout vertexLayout;
        bgfx::VertexBufferHandle vertexBuffer;
        bgfx::IndexBufferHandle indexBuffer;
        bgfx::ProgramHandle program;


        bool update(UpdateEvent* event) {
            BIGG_PROFILE_RENDERER_FUNCTION;

            static const uint32_t viewID = 0;
            static double counter = 0.0f;
            counter += event->m_delta;

            auto view = m_registry.view<Mesh, Transform>();
            for(const auto& [entity, mesh, transform] : view.each()) {
                BIGG_PROFILE_RENDERER_SCOPE("foreach mesh");

                {
                    const glm::vec3 at = {0.0f, 0.0f, 0.0f};
                    const glm::vec3 eye = {0.0f, 0.0f, -5.0f};

                    glm::mat4 view = glm::lookAtLH(eye, at, {0.0f, 1.0f, 0.0f});
                    glm::mat4 proj = glm::perspectiveLH(glm::radians(60.0f), (float) 720 / (float) 600, 0.1f, 100.0f);

                    bgfx::setViewTransform(viewID, glm::value_ptr(view), glm::value_ptr(proj));

                    glm::mat4 trans = glm::mat4(1.0f);
                    glm::mat4 rot_ = glm::mat4(1.0f);
                    rot_ = glm::rotate(rot_, (float)counter * 0.25f, {0.0f, 0.0f, 1.0f});
                    glm::vec4 rotDir = glm::vec4(0.0f, 0.3f, 0.4f, 0.0f) * rot_;
                    trans = glm::rotate(trans, (float)counter, glm::vec3(rotDir));
                    // TODO rotate by transform.rotation
                    trans = glm::scale(trans, transform.scale);
                    trans = glm::translate(trans, transform.position);
                    bgfx::setTransform(glm::value_ptr(trans));

                }
//
//                float transform[16];
//                bx::mtxRotateXY(transform, counter, 0.943f * counter);
//                bgfx::setTransform(transform);

                bgfx::setVertexBuffer(0, vertexBuffer);
                bgfx::setIndexBuffer(indexBuffer);

                bgfx::submit(viewID, program);
            }

            return false;
        }

        void shutdown() {
            bgfx::destroy(vertexBuffer);
            bgfx::destroy(indexBuffer);
            bgfx::destroy(program);
        }

        const bgfx::Memory* loadMem(bx::AllocatorI* allocator, const char* filepath) {
            BIGG_PROFILE_RENDERER_FUNCTION;
            bx::FileReaderI* fileReader = BX_NEW(allocator, bx::FileReader);
            BIGG_ASSERT(bx::open(fileReader, filepath), "Failed to load {}", filepath);
            uint32_t size = (uint32_t)bx::getSize(fileReader);
            const bgfx::Memory* mem = bgfx::alloc(size+1);
            bx::read(fileReader, mem->data, size, bx::ErrorAssert{});
            bx::close(fileReader);
            mem->data[mem->size-1] = '\0';
            BX_DELETE(allocator, fileReader);
            return mem;
        }

    private:
        ContextI* m_context;
        entt::registry& m_registry;
    };


} // namespace BIGGEngine
