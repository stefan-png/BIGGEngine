//
// Created by Stefan Antoszko on 2022-06-23.
//

#include "RenderMeshComponents.hpp"

#include "../Context.hpp"
#include "RenderUtils.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho()
#include <glm/gtc/type_ptr.hpp>         // for glm::value_ptr() (convert mat4 to float[16])

namespace BIGGEngine {
namespace RenderMeshComponents {
namespace {

    Mesh g_mesh;
    bgfx::VertexLayout g_vertexLayout;
    bgfx::VertexBufferHandle g_vertexBuffer;
    bgfx::IndexBufferHandle g_indexBuffer;
    bgfx::ProgramHandle g_program;

    bool onWindowCreate(WindowCreateEvent e) {
        g_vertexLayout.begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();

        g_vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(g_mesh.m_vertices.data(), sizeof(ImplVertex) * g_mesh.m_vertices.size()), g_vertexLayout);
        g_indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(g_mesh.m_indices.data(), sizeof(ImplIndex) * g_mesh.m_indices.size()));

        bx::AllocatorI* allocator = Context::getAllocator();
        bgfx::ShaderHandle vs = bgfx::createShader(RenderUtils::loadMem(allocator, "../thirdparty/bgfx/examples/runtime/shaders/metal/vs_cubes.bin"));
        bgfx::ShaderHandle fs = bgfx::createShader(RenderUtils::loadMem(allocator, "../thirdparty/bgfx/examples/runtime/shaders/metal/fs_cubes.bin"));
        g_program = bgfx::createProgram(vs, fs, true);

        bgfx::destroy(vs);
        bgfx::destroy(fs);
        return false;
    }

    bool onUpdate(UpdateEvent e) {
        BIGG_PROFILE_RENDER_FUNCTION;

        static const uint32_t viewID = 0;
        static double counter = 0.0f;
        counter += e.m_delta;

        auto view = ECS::get().view<Mesh, Transform>();
        for(const auto& [entity, mesh, transform] : view.each()) {
            BIGG_PROFILE_RENDER_SCOPE("foreach mesh");

            {
                const glm::vec3 at = {0.0f, 0.0f, 0.0f};
                const glm::vec3 eye = {0.0f, 0.0f, -10.0f};

                glm::ivec2 framesize = Context::getWindowFramebufferSize();

                glm::mat4 view = glm::lookAtLH(eye, at, {0.0f, 1.0f, 0.0f});
                glm::mat4 proj = glm::perspectiveLH(glm::radians(30.0f), (float) framesize.x / (float) framesize.y, 0.1f, 100.0f);

                bgfx::setViewTransform(viewID, glm::value_ptr(view), glm::value_ptr(proj));

                glm::mat4 trans = glm::mat4(1.0f);
                glm::mat4 rot_ = glm::mat4(1.0f);
                rot_ = glm::rotate(rot_, (float)counter * 0.25f, {0.0f, 0.0f, 1.0f});
                glm::vec4 rotDir = glm::vec4(0.0f, 0.3f, 0.4f, 0.0f) * rot_;
                // TODO rotate by transform.rotation
                trans = glm::translate(trans, transform.position);
                trans = glm::rotate(trans, (float)counter, glm::vec3(rotDir));
                trans = glm::scale(trans, transform.scale);
                bgfx::setTransform(glm::value_ptr(trans));

            }
//
//                float transform[16];
//                bx::mtxRotateXY(transform, counter, 0.943f * counter);
//                bgfx::setTransform(transform);

            bgfx::setVertexBuffer(0, g_vertexBuffer);
            bgfx::setIndexBuffer(g_indexBuffer);

            bgfx::submit(viewID, g_program);
        }

        return false;
    }

    bool onWindowShouldClose(WindowShouldCloseEvent e) {
        bgfx::destroy(g_vertexBuffer);
        bgfx::destroy(g_indexBuffer);
        bgfx::destroy(g_program);

        return false;
    }

}


    void init() {
        BIGG_PROFILE_INIT_FUNCTION;

        //g_renderMeshComponentsPriority

        Events::subscribe<WindowCreateEvent>(g_renderMeshComponentsPriority, onWindowCreate);
        Events::subscribe<UpdateEvent>(g_renderMeshComponentsPriority, onUpdate);
        Events::subscribe<WindowShouldCloseEvent>(g_renderMeshComponentsPriority, onWindowShouldClose);
    }

}// namespace RenderMeshComponents
} // namespace BIGGEngine