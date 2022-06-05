#pragma once

#include "../Core.hpp"
#include "../ContextI.hpp"

#include <imgui.h>
#include <bgfx/bgfx.h>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::ortho()
#include <glm/gtc/type_ptr.hpp>         // for glm::value_ptr() (convert mat4 to float[16])

// for reading shader from disk
#include <bx/file.h>
#include <bx/readerwriter.h>

#include <memory>   // for unique_ptr

#define BIGG_PROFILE_UI_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("ui")
#define BIGG_PROFILE_UI_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("ui", _format, ##__VA_ARGS__)

namespace BIGGEngine {

static ImGuiKey KeyEnumToImGuiKey(KeyEnum key)
{
    switch (key)
    {
        case KeyEnum::Tab: return ImGuiKey_Tab;
        case KeyEnum::Left: return ImGuiKey_LeftArrow;
        case KeyEnum::Right: return ImGuiKey_RightArrow;
        case KeyEnum::Up: return ImGuiKey_UpArrow;
        case KeyEnum::Down: return ImGuiKey_DownArrow;
        case KeyEnum::PageUp: return ImGuiKey_PageUp;
        case KeyEnum::PageDown: return ImGuiKey_PageDown;
        case KeyEnum::Home: return ImGuiKey_Home;
        case KeyEnum::End: return ImGuiKey_End;
        case KeyEnum::Insert: return ImGuiKey_Insert;
        case KeyEnum::Delete: return ImGuiKey_Delete;
        case KeyEnum::Backspace: return ImGuiKey_Backspace;
        case KeyEnum::Space: return ImGuiKey_Space;
        case KeyEnum::Enter: return ImGuiKey_Enter;
        case KeyEnum::Escape: return ImGuiKey_Escape;
        case KeyEnum::Apostrophe: return ImGuiKey_Apostrophe;
        case KeyEnum::Comma: return ImGuiKey_Comma;
        case KeyEnum::Minus: return ImGuiKey_Minus;
        case KeyEnum::Period: return ImGuiKey_Period;
        case KeyEnum::Slash: return ImGuiKey_Slash;
        case KeyEnum::Semicolon: return ImGuiKey_Semicolon;
        case KeyEnum::Equal: return ImGuiKey_Equal;
        case KeyEnum::LeftBracket: return ImGuiKey_LeftBracket;
        case KeyEnum::Backslash: return ImGuiKey_Backslash;
        case KeyEnum::RightBracket: return ImGuiKey_RightBracket;
        case KeyEnum::GraveAccent: return ImGuiKey_GraveAccent;
        case KeyEnum::CapsLock: return ImGuiKey_CapsLock;
        case KeyEnum::ScrollLock: return ImGuiKey_ScrollLock;
        case KeyEnum::NumLock: return ImGuiKey_NumLock;
        case KeyEnum::PrintScreen: return ImGuiKey_PrintScreen;
        case KeyEnum::Pause: return ImGuiKey_Pause;
        case KeyEnum::NumpadZero: return ImGuiKey_Keypad0;
        case KeyEnum::NumpadOne: return ImGuiKey_Keypad1;
        case KeyEnum::NumpadTwo: return ImGuiKey_Keypad2;
        case KeyEnum::NumpadThree: return ImGuiKey_Keypad3;
        case KeyEnum::NumpadFour: return ImGuiKey_Keypad4;
        case KeyEnum::NumpadFive: return ImGuiKey_Keypad5;
        case KeyEnum::NumpadSix: return ImGuiKey_Keypad6;
        case KeyEnum::NumpadSeven: return ImGuiKey_Keypad7;
        case KeyEnum::NumpadEight: return ImGuiKey_Keypad8;
        case KeyEnum::NumpadNine: return ImGuiKey_Keypad9;
        case KeyEnum::NumpadDecimal: return ImGuiKey_KeypadDecimal;
        case KeyEnum::NumpadDivide: return ImGuiKey_KeypadDivide;
        case KeyEnum::NumpadMultiply: return ImGuiKey_KeypadMultiply;
        case KeyEnum::NumpadSubtract: return ImGuiKey_KeypadSubtract;
        case KeyEnum::NumpadAdd: return ImGuiKey_KeypadAdd;
        case KeyEnum::NumpadEnter: return ImGuiKey_KeypadEnter;
        case KeyEnum::NumpadEqual: return ImGuiKey_KeypadEqual;
        case KeyEnum::LeftShift: return ImGuiKey_LeftShift;
        case KeyEnum::LeftControl: return ImGuiKey_LeftCtrl;
        case KeyEnum::LeftAlt: return ImGuiKey_LeftAlt;
        case KeyEnum::LeftSuper: return ImGuiKey_LeftSuper;
        case KeyEnum::RightShift: return ImGuiKey_RightShift;
        case KeyEnum::RightControl: return ImGuiKey_RightCtrl;
        case KeyEnum::RightAlt: return ImGuiKey_RightAlt;
        case KeyEnum::RightSuper: return ImGuiKey_RightSuper;
        case KeyEnum::Menu: return ImGuiKey_Menu;
        case KeyEnum::Zero: return ImGuiKey_0;
        case KeyEnum::One: return ImGuiKey_1;
        case KeyEnum::Two: return ImGuiKey_2;
        case KeyEnum::Three: return ImGuiKey_3;
        case KeyEnum::Four: return ImGuiKey_4;
        case KeyEnum::Five: return ImGuiKey_5;
        case KeyEnum::Six: return ImGuiKey_6;
        case KeyEnum::Seven: return ImGuiKey_7;
        case KeyEnum::Eight: return ImGuiKey_8;
        case KeyEnum::Nine: return ImGuiKey_9;
        case KeyEnum::A: return ImGuiKey_A;
        case KeyEnum::B: return ImGuiKey_B;
        case KeyEnum::C: return ImGuiKey_C;
        case KeyEnum::D: return ImGuiKey_D;
        case KeyEnum::E: return ImGuiKey_E;
        case KeyEnum::F: return ImGuiKey_F;
        case KeyEnum::G: return ImGuiKey_G;
        case KeyEnum::H: return ImGuiKey_H;
        case KeyEnum::I: return ImGuiKey_I;
        case KeyEnum::J: return ImGuiKey_J;
        case KeyEnum::K: return ImGuiKey_K;
        case KeyEnum::L: return ImGuiKey_L;
        case KeyEnum::M: return ImGuiKey_M;
        case KeyEnum::N: return ImGuiKey_N;
        case KeyEnum::O: return ImGuiKey_O;
        case KeyEnum::P: return ImGuiKey_P;
        case KeyEnum::Q: return ImGuiKey_Q;
        case KeyEnum::R: return ImGuiKey_R;
        case KeyEnum::S: return ImGuiKey_S;
        case KeyEnum::T: return ImGuiKey_T;
        case KeyEnum::U: return ImGuiKey_U;
        case KeyEnum::V: return ImGuiKey_V;
        case KeyEnum::W: return ImGuiKey_W;
        case KeyEnum::X: return ImGuiKey_X;
        case KeyEnum::Y: return ImGuiKey_Y;
        case KeyEnum::Z: return ImGuiKey_Z;
        case KeyEnum::F1: return ImGuiKey_F1;
        case KeyEnum::F2: return ImGuiKey_F2;
        case KeyEnum::F3: return ImGuiKey_F3;
        case KeyEnum::F4: return ImGuiKey_F4;
        case KeyEnum::F5: return ImGuiKey_F5;
        case KeyEnum::F6: return ImGuiKey_F6;
        case KeyEnum::F7: return ImGuiKey_F7;
        case KeyEnum::F8: return ImGuiKey_F8;
        case KeyEnum::F9: return ImGuiKey_F9;
        case KeyEnum::F10: return ImGuiKey_F10;
        case KeyEnum::F11: return ImGuiKey_F11;
        case KeyEnum::F12: return ImGuiKey_F12;
        default: return ImGuiKey_None;
    }
}

struct RenderUIData {
public:
    bgfx::VertexLayout  m_layout;
    bgfx::ProgramHandle m_program;
    bgfx::ProgramHandle m_imageProgram;
    bgfx::TextureHandle m_texture;
    bgfx::UniformHandle s_tex;
    bgfx::UniformHandle u_imageLodEnabled;

public:
    RenderUIData() {

        bgfx::RendererType::Enum type = bgfx::getRendererType();
        bx::AllocatorI* allocator = ContextI::getInstance()->getAllocator();
        m_program = bgfx::createProgram(
            bgfx::createShader(loadMem(allocator, "../res/shaders/vs_ocornut_imgui.bin"))
            , bgfx::createShader(loadMem(allocator, "../res/shaders/fs_ocornut_imgui.bin"))
            , true
        );
        u_imageLodEnabled = bgfx::createUniform("u_imageLodEnabled", bgfx::UniformType::Vec4);
        m_imageProgram = bgfx::createProgram(
            bgfx::createShader(loadMem(allocator, "../res/shaders/vs_imgui_image.bin"))
            , bgfx::createShader(loadMem(allocator, "../res/shaders/fs_imgui_image.bin"))
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
        uint8_t* data;
        int32_t width;
        int32_t height;
        ImGuiIO& io = ImGui::GetIO();
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
    }
    ~RenderUIData() {
        bgfx::destroy(s_tex);
        bgfx::destroy(m_texture);
        bgfx::destroy(u_imageLodEnabled);
        bgfx::destroy(m_imageProgram);
        bgfx::destroy(m_program);
    }
private:
    const bgfx::Memory* loadMem(bx::AllocatorI* allocator, const char* filepath) {
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
};

struct RenderUI {
public:
    RenderUI() {
        BIGG_PROFILE_INIT_FUNCTION;

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        BIGG_ASSERT(io.BackendPlatformUserData == nullptr, "Already initialized a platform backend!")
//        io.BackendPlatformUserData = context;
        io.BackendPlatformName = "BIGG Engine";
        io.BackendRendererName = "bgfx";

        setStyle();

        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    
        io.SetClipboardTextFn = [](void* userData, const char* string){
            ContextI::getInstance()->setClipboardString(string);
        };
        io.GetClipboardTextFn = [](void* userData) -> const char*{
            return ContextI::getInstance()->getClipboardString().c_str();
        };

        ContextI* ctx = ContextI::getInstance();

        // TODO cursors
        bool subscribed = ctx->subscribe(g_renderUIBeginPriority, [this](Events* event) -> bool {
            switch(event->m_type) {
                case Events::EventType::WindowCreate:
                    return handleWindowCreateEvent();
                case Events::EventType::WindowSize:
                    return handleWindowSizeEvent();
                case Events::EventType::WindowFocus:
                    return handleWindowFocusEvent(static_cast<WindowFocusEvent*>(event));       
                case Events::EventType::MouseEnter:
                    return handleMouseEnterEvent(static_cast<MouseEnterEvent*>(event));
                case Events::EventType::MousePosition:
                    return handleMousePositionEvent(static_cast<MousePositionEvent*>(event));
                case Events::EventType::MouseButton:
                    return handleMouseButtonEvent(static_cast<MouseButtonEvent*>(event));
                case Events::EventType::Scroll:
                    return handleScrollEvent(static_cast<ScrollEvent*>(event));
                case Events::EventType::Key:
                    return handleKeyEvent(static_cast<KeyEvent*>(event));
                case Events::EventType::Char:
                    return handleCharEvent(static_cast<CharEvent*>(event));
                case Events::EventType::Update:
                    return handleEarlyUpdateEvent(static_cast<UpdateEvent*>(event)->m_delta);
                // case Events::EventType::Monitor:
                // TODO add monitor callback for "docking" branch of imgui
                default:
                    return false;
            }
        });
        BIGG_ASSERT(subscribed, "priority level {} is not available!", g_renderUIBeginPriority);

        subscribed = ctx->subscribe(g_renderUIEndPriority, [this](Events* event) -> bool {
            if(event->m_type != Events::EventType::Update)
                return false;

            return handleLateUpdateEvent();
        });
        BIGG_ASSERT(subscribed, "priority level {} is not available!", g_renderUIEndPriority);
    }
    
    ~RenderUI() {
        BIGG_PROFILE_SHUTDOWN_FUNCTION;

        ImGui::DestroyContext();
    }

private:
    void updateKeyModifiers(ModsEnum mods) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddKeyEvent(ImGuiKey_ModCtrl, (mods & ModsEnum::Control) != 0);
        io.AddKeyEvent(ImGuiKey_ModShift, (mods & ModsEnum::Shift) != 0);
        io.AddKeyEvent(ImGuiKey_ModAlt, (mods & ModsEnum::Alt) != 0);
        io.AddKeyEvent(ImGuiKey_ModSuper, (mods & ModsEnum::Super) != 0);
    }

    void setStyle() {
        // modified from olekristensen/ledSynthmaster
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding            = ImVec2(15, 15);
        style.WindowRounding           = 5.0f;
        style.FramePadding             = ImVec2(5, 5);
        style.FrameRounding            = 4.0f;
        style.ItemSpacing              = ImVec2(12, 8);
        style.ItemInnerSpacing         = ImVec2(8, 6);
        style.IndentSpacing            = 25.0f;
        style.ScrollbarSize            = 15.0f;
        style.ScrollbarRounding        = 9.0f;
        style.GrabMinSize              = 5.0f;
        style.GrabRounding             = 3.0f;

        style.Colors[ImGuiCol_Text]                  = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.40f, 0.39f, 0.38f, 0.77f);
        style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.92f, 0.91f, 0.88f, 0.70f);
        style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.92f, 0.91f, 0.88f, 0.92f);
        style.Colors[ImGuiCol_Border]                = ImVec4(0.84f, 0.83f, 0.80f, 0.65f);
        style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.96f, 0.87f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.97f, 0.80f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_TitleBg]               = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
        style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(1.00f, 0.98f, 0.95f, 0.47f);
        style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.00f, 0.00f, 0.00f, 0.21f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.555f, 0.779f, 0.474f, 1.000f);
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
        style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
        style.Colors[ImGuiCol_Button]                = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
        style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.96f, 0.87f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.97f, 0.80f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_Header]                = ImVec4(0.55f, 0.78f, 0.47f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.555f, 0.779f, 0.474f, 1.000f);
        style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
        style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);

        style.Colors[ImGuiCol_Tab]                    = ImVec4(0.86f, 1.00f, 0.81f, 1.00f);
        style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.55f, 0.78f, 0.47f, 1.00f);
        style.Colors[ImGuiCol_TabActive]              = ImVec4(0.55f, 0.78f, 0.47f, 1.00f);

        // add fonts
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("../res/fonts/Hasklig-Semibold.otf", 18);
        // io.Fonts->AddFontFromFileTTF("../res/fonts/Arial.ttf", 18);
    }

    bool handleWindowCreateEvent() {
        BIGG_PROFILE_UI_FUNCTION;

        ImGuiIO& io = ImGui::GetIO();
        
        m_data = std::make_unique<RenderUIData>();

        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        io.DisplaySize = ContextI::getInstance()->getWindowFramebufferSize();
        return false;
    }
    bool handleWindowSizeEvent() {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ContextI::getInstance()->getWindowFramebufferSize();
        // io.DisplayFramebufferScale = m_context->getWindowFramebufferSize() / m_context->getWindowSize();
        return false;
    }
    bool handleWindowFocusEvent(WindowFocusEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(e->m_focused);
        return false;  
    }
    bool handleMouseEnterEvent(MouseEnterEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        if(e->m_entered) {
            // TODO add lastValidMOusePos member
            // io.AddMousePosEvent(m_context->lastValidMousePos.x, m_context->lastValidMousePos.y);
            return false; // TODO change to true
        }
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        return false;
    }
    bool handleMousePositionEvent(MousePositionEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(e->m_mousePosition.x, e->m_mousePosition.y);
        return false;
    }
    bool handleMouseButtonEvent(MouseButtonEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        updateKeyModifiers(e->m_mods);

        io.AddMouseButtonEvent((int)e->m_button, e->m_action == ActionEnum::Press);
        return io.WantCaptureMouse;
    }
    bool handleScrollEvent(ScrollEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(e->m_delta.x, e->m_delta.y);
        return io.WantCaptureMouse;
    }
    bool handleKeyEvent(KeyEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        updateKeyModifiers(e->m_mods);
    
        if(e->m_action == ActionEnum::Repeat) {
            return false;
        }
        io.AddKeyEvent(KeyEnumToImGuiKey(e->m_key), e->m_action == ActionEnum::Press);
        return io.WantCaptureKeyboard;
    }
    bool handleCharEvent(CharEvent* e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(e->m_codepoint);
        return io.WantCaptureKeyboard;
    }
    bool handleEarlyUpdateEvent(double delta) {
        BIGG_PROFILE_UI_FUNCTION;
        ImGuiIO& io = ImGui::GetIO();

        io.DeltaTime = delta;
        ImGui::NewFrame();
        return false;
    }
    bool handleLateUpdateEvent() {
        BIGG_PROFILE_UI_FUNCTION;

        BIGG_ASSERT(m_data != nullptr, "m_data isn't initialized!");

        // render the imgui things using bgfx
        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        uint32_t viewID = 1;

		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		int fb_height = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return false;

		bgfx::setViewName(viewID, "ImGui");
		bgfx::setViewMode(viewID, bgfx::ViewMode::Sequential);

		const bgfx::Caps* caps = bgfx::getCaps();
		{

			float x = drawData->DisplayPos.x;
			float y = drawData->DisplayPos.y;
			float width = drawData->DisplaySize.x;
			float height = drawData->DisplaySize.y;

            glm::mat4 ortho = glm::ortho(x, x + width, y + height, y, 0.0f, 1000.0f);
			bgfx::setViewTransform(viewID, NULL, glm::value_ptr(ortho));
			bgfx::setViewRect(viewID, 0, 0, uint16_t(width), uint16_t(height) );
        }

		const ImVec2 clipPos   = drawData->DisplayPos;       // (0,0) unless using multi-viewports
		const ImVec2 clipScale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		// Render command lists
		for (int32_t ii = 0, num = drawData->CmdListsCount; ii < num; ++ii)
		{
			bgfx::TransientVertexBuffer tvb;
			bgfx::TransientIndexBuffer tib;

			const ImDrawList* drawList = drawData->CmdLists[ii];
			uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
			uint32_t numIndices  = (uint32_t)drawList->IdxBuffer.size();

	        if(!(numVertices == bgfx::getAvailTransientVertexBuffer(numVertices, m_data->m_layout)
		        && (0 == numIndices || numIndices == bgfx::getAvailTransientIndexBuffer(numIndices) ))) {
				// not enough space in transient buffer just quit drawing the rest...
                BIGG_LOG_WARN("Not enough available Transient Vertex Buffers! Stopping rendering UI");
				break;
			}

			bgfx::allocTransientVertexBuffer(&tvb, numVertices, m_data->m_layout);
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

					bgfx::TextureHandle th = m_data->m_texture;
					bgfx::ProgramHandle program = m_data->m_program;

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
							bgfx::setUniform(m_data->u_imageLodEnabled, lodEnabled);
							program = m_data->m_imageProgram;
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
						encoder->setTexture(0, m_data->s_tex, th);
						encoder->setVertexBuffer(0, &tvb, cmd->VtxOffset, numVertices);
						encoder->setIndexBuffer(&tib, cmd->IdxOffset, cmd->ElemCount);
						encoder->submit(viewID, program);
					}
				}
			}

			bgfx::end(encoder);
		}
        return false;
    }

private:
    std::unique_ptr<RenderUIData> m_data;
};

} // namespace BIGGEngine