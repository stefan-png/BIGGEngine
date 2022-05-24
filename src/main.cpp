
#include <spdlog/spdlog.h>
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

#include "../bgfx/examples/common/bgfx_utils.h"
#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>

#include "NativeWindowHack.hpp"

#include <bx/math.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <bx/string.h>

const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 720;

struct Vertex {
    float x, y, z;
    uint32_t color; //ABGR
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

#include "../bgfx/examples/common/imgui/vs_ocornut_imgui.bin.h"
#include "../bgfx/examples/common/imgui/fs_ocornut_imgui.bin.h"
#include "../bgfx/examples/common/imgui/vs_imgui_image.bin.h"
#include "../bgfx/examples/common/imgui/fs_imgui_image.bin.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] =
        {
                BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
                BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),

                BGFX_EMBEDDED_SHADER_END()
        };

bgfx::VertexLayout  m_layout;
bgfx::ProgramHandle m_program;
bgfx::ProgramHandle m_imageProgram;
bgfx::TextureHandle m_texture;
bgfx::UniformHandle s_tex;
bgfx::UniformHandle u_imageLodEnabled;

void render(ImDrawData* _drawData)
	{

        
        uint32_t m_viewId = 1;

		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(_drawData->DisplaySize.x * _drawData->FramebufferScale.x);
		int fb_height = (int)(_drawData->DisplaySize.y * _drawData->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;



		bgfx::setViewName(m_viewId, "ImGui");
		bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);

		const bgfx::Caps* caps = bgfx::getCaps();
		{
			float ortho[16];
			float x = _drawData->DisplayPos.x;
			float y = _drawData->DisplayPos.y;
			float width = _drawData->DisplaySize.x;
			float height = _drawData->DisplaySize.y;

			bx::mtxOrtho(ortho, x, x + width, y + height, y, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
			bgfx::setViewTransform(m_viewId, NULL, ortho);
			bgfx::setViewRect(m_viewId, 0, 0, uint16_t(width), uint16_t(height) );

            //TODO only reset if you NEED to reset (ie window size changed)
            bgfx::reset(width, height, BGFX_RESET_VSYNC);
            //also set the view rect for view #1 aka 3d
            bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
        }

		const ImVec2 clipPos   = _drawData->DisplayPos;       // (0,0) unless using multi-viewports
		const ImVec2 clipScale = _drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		// Render command lists
		for (int32_t ii = 0, num = _drawData->CmdListsCount; ii < num; ++ii)
		{
			bgfx::TransientVertexBuffer tvb;
			bgfx::TransientIndexBuffer tib;

			const ImDrawList* drawList = _drawData->CmdLists[ii];
			uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
			uint32_t numIndices  = (uint32_t)drawList->IdxBuffer.size();

			if (!checkAvailTransientBuffers(numVertices, m_layout, numIndices) )
			{
				// not enough space in transient buffer just quit drawing the rest...
				break;
			}

			bgfx::allocTransientVertexBuffer(&tvb, numVertices, m_layout);
			bgfx::allocTransientIndexBuffer(&tib, numIndices, sizeof(ImDrawIdx) == 4);

			ImDrawVert* verts = (ImDrawVert*)tvb.data;
			bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert) );

			ImDrawIdx* indices = (ImDrawIdx*)tib.data;
			bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx) );

			bgfx::Encoder* encoder = bgfx::begin();

			for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
			{
				if (cmd->UserCallback)
				{
					cmd->UserCallback(drawList, cmd);
				}
				else if (0 != cmd->ElemCount)
				{
					uint64_t state = 0
						| BGFX_STATE_WRITE_RGB
						| BGFX_STATE_WRITE_A
						| BGFX_STATE_MSAA
						;

					bgfx::TextureHandle th = m_texture;
					bgfx::ProgramHandle program = m_program;

                    //TODO FIX THIS stupid define
                    #define IMGUI_FLAGS_ALPHA_BLEND 0x01

					if (NULL != cmd->TextureId)
					{
						union { ImTextureID ptr; struct { bgfx::TextureHandle handle; uint8_t flags; uint8_t mip; } s; } texture = { cmd->TextureId };
						state |= 0 != (IMGUI_FLAGS_ALPHA_BLEND & texture.s.flags)
							? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
							: BGFX_STATE_NONE
							;
						th = texture.s.handle;
						if (0 != texture.s.mip)
						{
							const float lodEnabled[4] = { float(texture.s.mip), 1.0f, 0.0f, 0.0f };
							bgfx::setUniform(u_imageLodEnabled, lodEnabled);
							program = m_imageProgram;
						}
					}
					else
					{
						state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
					}

					// Project scissor/clipping rectangles into framebuffer space
					ImVec4 clipRect;
					clipRect.x = (cmd->ClipRect.x - clipPos.x) * clipScale.x;
					clipRect.y = (cmd->ClipRect.y - clipPos.y) * clipScale.y;
					clipRect.z = (cmd->ClipRect.z - clipPos.x) * clipScale.x;
					clipRect.w = (cmd->ClipRect.w - clipPos.y) * clipScale.y;

					if (clipRect.x <  fb_width
					&&  clipRect.y <  fb_height
					&&  clipRect.z >= 0.0f
					&&  clipRect.w >= 0.0f)
					{
						const uint16_t xx = uint16_t(bx::max(clipRect.x, 0.0f) );
						const uint16_t yy = uint16_t(bx::max(clipRect.y, 0.0f) );
						encoder->setScissor(xx, yy
								, uint16_t(bx::min(clipRect.z, 65535.0f)-xx)
								, uint16_t(bx::min(clipRect.w, 65535.0f)-yy)
								);

						encoder->setState(state);
						encoder->setTexture(0, s_tex, th);
						encoder->setVertexBuffer(0, &tvb, cmd->VtxOffset, numVertices);
						encoder->setIndexBuffer(&tib, cmd->IdxOffset, cmd->ElemCount);
						encoder->submit(m_viewId, program);
					}
				}
			}

			bgfx::end(encoder);
		}
    }

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   // important so that window doesn't go black as soon as I move it
    // glfwSwapInterval(1);    //enable vsync
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ybadabadoo", nullptr, nullptr);

    void* cocoaWindow = glfwGetCocoaWindow(window);
    // HACK: adds a CAMetalLayer to a NSWindow and returns the layer.
    void* metalLayer = addMetalLayerToCocoaWindow(cocoaWindow);

    bgfx::PlatformData pd;
    pd.ndt = nullptr;
    pd.context = nullptr;
    pd.backBuffer = nullptr;
    pd.backBufferDS = nullptr;
    pd.nwh = metalLayer;

    bgfx::Init init;
    init.platformData = pd;
    init.debug = false;
    init.resolution.width = WINDOW_WIDTH;
    init.resolution.height = WINDOW_HEIGHT;
    // init.resolution.reset = BGFX_RESET_VSYNC;
    init.vendorId = BGFX_PCI_ID_APPLE;
    init.type = bgfx::RendererType::Metal;

    bgfx::init(init);

    spdlog::info("inited bgfx!");

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x939762ff);
    bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // bgfx::setViewRect(1, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    bgfx::VertexLayout vertexLayout;
    vertexLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
    bgfx::VertexBufferHandle vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), vertexLayout);
    bgfx::IndexBufferHandle indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

    bgfx::ShaderHandle vs = bgfx::createShader(loadMem("../thirdparty/bgfx/examples/runtime/shaders/metal/vs_cubes.bin"));
    bgfx::ShaderHandle fs = bgfx::createShader(loadMem("../thirdparty/bgfx/examples/runtime/shaders/metal/fs_cubes.bin"));
    bgfx::ProgramHandle program = bgfx::createProgram(vs, fs, true);

    //from the spec: "Once a shader program is created with _handle, it is safe to destroy that shader"
    bgfx::destroy(vs);
    bgfx::destroy(fs);

    // ----------------- IMGUI SETUP ---------------------
    // Application init: create a dear imgui context, setup some options, load fonts
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
    // TODO: Fill optional fields of the io structure later.

    bgfx::RendererType::Enum type = bgfx::getRendererType();
    m_program = bgfx::createProgram(
            bgfx::createShader(loadMem("../res/shaders/vs_ocornut_imgui.bin"))
            , bgfx::createShader(loadMem("../res/shaders/fs_ocornut_imgui.bin"))
            , true
    );

    u_imageLodEnabled = bgfx::createUniform("u_imageLodEnabled", bgfx::UniformType::Vec4);
    m_imageProgram = bgfx::createProgram(
            bgfx::createShader(loadMem("../res/shaders/vs_imgui_image.bin"))
            , bgfx::createShader(loadMem("../res/shaders/fs_imgui_image.bin"))
            , true
    );

    m_layout
            .begin()
            .add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
            .end();

    s_tex = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

    // TODO: Load TTF/OTF fonts if you don't want to use the default font.
    // Build and load the texture atlas into a texture
    // (In the examples/ app this is usually done within the ImGui_ImplXXX_Init() function from one of the demo Renderer)
    uint8_t* data;
    int32_t width;
    int32_t height;
    io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);
    m_texture = bgfx::createTexture2D(
            (uint16_t)width
            , (uint16_t)height
            , false
            , 1
            , bgfx::TextureFormat::BGRA8
            , 0
            , bgfx::copy(data, width*height*4)
    );

    ImGui_ImplGlfw_InitForOther(window, true);

    unsigned int counter = 0;
    while(!glfwWindowShouldClose(window)) {
        glfwWaitEventsTimeout(0.05);

        if(counter % 60 == 0) {
            spdlog::info("Frame: {}", counter);
        }

        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        // ImGui::ShowDemoWindow();
        // ImGui::Text("Hello, World!");
        // ImGui::Render();
        // render(ImGui::GetDrawData());

/*
        const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
        const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
        float view[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        float aspectRatio = (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT;
        bx::mtxProj(proj, 60.0f, aspectRatio, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, &view, &proj);
        float transform[16];
        bx::mtxRotateXY(transform, counter*0.005f, counter*0.0091f);
        bgfx::setTransform(transform);

        bgfx::setVertexBuffer(0, vertexBuffer);
        bgfx::setIndexBuffer(indexBuffer);

        // bgfx::submit(0, program); 
*/
        bgfx::touch(0); //dummy draw call to view 0 so bg is drawn
        bgfx::frame();

        counter++;
    }

    bgfx::destroy(vertexBuffer);
    bgfx::destroy(indexBuffer);
    bgfx::destroy(program);

    //  --------    Destroy Imgui   ------
    ImGui::DestroyContext();
    bgfx::destroy(s_tex);
    bgfx::destroy(m_texture);

    bgfx::destroy(u_imageLodEnabled);
    bgfx::destroy(m_imageProgram);
    bgfx::destroy(m_program);

    

    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    spdlog::info("Shutting down!");
    return 0;
}