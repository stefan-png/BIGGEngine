//
// Created by Stefan Antoszko on 2022-05-27.
//

// TODO __tostring metamethods for all lua types (Entity, Component, Vector)
// - add Context functions in lua (getWindowSize, getMousePos, etc) 80%
// - add rest of Components 0%
// - add rest of event callbacks 0%

#include "Script.hpp"
#include "Macros.hpp"

#define BIGG_PROFILE_SCRIPT_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("script")
#define BIGG_PROFILE_SCRIPT_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("script", _format, ##__VA_ARGS__)

#include <utility> // for helper functions

#define CHECK_LUA(L, x) if((x)) { \
    lua_error(L);                 \
}

namespace BIGGEngine {
namespace Scripting {
namespace {

    bool isDefaultName(const char* s) {
        return
                strcmp(s, "_G") == 0 ||
                strcmp(s, "_VERSION") == 0 ||
                strcmp(s, "assert") == 0 ||
                strcmp(s, "collectgarbage") == 0 ||
                strcmp(s, "dofile") == 0 ||
                strcmp(s, "error") == 0 ||
                strcmp(s, "getmetatable") == 0 ||
                strcmp(s, "ipairs") == 0 ||
                strcmp(s, "load") == 0 ||
                strcmp(s, "loadfile") == 0 ||
                strcmp(s, "next") == 0 ||
                strcmp(s, "pairs") == 0 ||
                strcmp(s, "pcall") == 0 ||
                strcmp(s, "print") == 0 ||
                strcmp(s, "rawequal") == 0 ||
                strcmp(s, "rawget") == 0 ||
                strcmp(s, "rawlen") == 0 ||
                strcmp(s, "rawset") == 0 ||
                strcmp(s, "require") == 0 ||
                strcmp(s, "select") == 0 ||
                strcmp(s, "setmetatable") == 0 ||
                strcmp(s, "tonumber") == 0 ||
                strcmp(s, "tostring") == 0 ||
                strcmp(s, "type") == 0 ||
                strcmp(s, "warn") == 0 ||
                strcmp(s, "xpcall") == 0 ||
                strcmp(s, "coroutine") == 0 ||
                strcmp(s, "debug") == 0 ||
                strcmp(s, "io") == 0 ||
                strcmp(s, "math") == 0 ||
                strcmp(s, "os") == 0 ||
                strcmp(s, "package") == 0 ||
                strcmp(s, "string") == 0 ||
                strcmp(s, "table") == 0 ||
                strcmp(s, "utf8") == 0 ||
                strcmp(s, "BIGGEngine") == 0;
    }

    struct Buffer {
        Buffer() = default;
        Buffer(void* data, size_t size) {
            BIGG_PROFILE_INIT_FUNCTION;
            if (size <= 0) return;
            m_data = malloc(size);
            memcpy(this->m_data, data, size);
            m_size = size;
        }
        ~Buffer() {
            BIGG_PROFILE_SCRIPT_FUNCTION;
            free(m_data);
        }

        void push(const void* data, size_t size) {
            BIGG_PROFILE_SCRIPT_FUNCTION;
            if (size <= 0) return;

            void *d = malloc(m_size + size);
            memcpy((char *) d + m_size, data, size);    // since can't do pointer arithmetic on void*

            memcpy(d, m_data, m_size);  // could copy nothing if m_size == 0
            free(m_data);               // could free nothing if m_data == nullptr

            m_data = d;
            m_size += size;
        }

        void* data() { return m_data; }
        size_t size() { return m_size; }

        friend int writer(lua_State* L, const void* p, size_t sz, void * ud);
        friend const char *reader (lua_State *L, void * ud, size_t *size);

    private:
        void* m_data = nullptr;
        size_t m_size = 0;
    };

    int writer(lua_State *L, const void *p, size_t sz, void *ud) {
        Buffer *buf = static_cast<Buffer *>(ud);
        buf->push(p, sz);
        return 0;
    }

    const char *reader(lua_State *, void *ud, size_t *size) {
        Buffer *buf = static_cast<Buffer *>(ud);
        *size = buf->size();
        return static_cast<const char *>(buf->data());
    }

    struct ScriptHandle {
        ScriptHandle(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority);
        ~ScriptHandle();

        // I'm expecting these to be hardcoded into the executable.
        entt::hashed_string::hash_type name;      // name to be registered
        Buffer m_buffer;            // buffer for caching the script.
        lua_State* m_luaState = nullptr;
    };

    // global state
    std::unordered_map<entt::hashed_string::hash_type, ScriptHandle> g_registeredScripts;

    // Library things

    // all the open_Xxx functions expect a stack with table 'BIGGEngine' at the top.
    void open_MouseButtonEnum(lua_State* L) {
        lua_createtable(L, 0, 11);      // create table MouseButtonEnum

        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Zero));
        lua_setfield(L, -2, "Zero");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::One));
        lua_setfield(L, -2, "One");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Two));
        lua_setfield(L, -2, "Two");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Three));
        lua_setfield(L, -2, "Three");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Four));
        lua_setfield(L, -2, "Four");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Five));
        lua_setfield(L, -2, "Five");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Six));
        lua_setfield(L, -2, "Six");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Seven));
        lua_setfield(L, -2, "Seven");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Left));
        lua_setfield(L, -2, "Left");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Middle));
        lua_setfield(L, -2, "Middle");
        lua_pushnumber(L, static_cast<lua_Number>(MouseButtonEnum::Right));
        lua_setfield(L, -2, "Right");

        lua_setfield(L, -2, "MouseButtonEnum"); // set BIGGEngine.MouseButtonEnum
    }

    void open_KeyEnum(lua_State* L) {
        lua_createtable(L, 0, 121);      // create table KeyEnum

        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Unknown));
        lua_setfield(L, -2, "Unknown");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Space));
        lua_setfield(L, -2, "Space");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Apostrophe));
        lua_setfield(L, -2, "Apostrophe");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Comma));
        lua_setfield(L, -2, "Comma");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Minus));
        lua_setfield(L, -2, "Minus");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Period));
        lua_setfield(L, -2, "Period");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Slash));
        lua_setfield(L, -2, "Slash");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Zero));
        lua_setfield(L, -2, "Zero");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::One));
        lua_setfield(L, -2, "One");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Two));
        lua_setfield(L, -2, "Two");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Three));
        lua_setfield(L, -2, "Three");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Four));
        lua_setfield(L, -2, "Four");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Five));
        lua_setfield(L, -2, "Five");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Six));
        lua_setfield(L, -2, "Six");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Seven));
        lua_setfield(L, -2, "Seven");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Eight));
        lua_setfield(L, -2, "Eight");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Nine));
        lua_setfield(L, -2, "Nine");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Semicolon));
        lua_setfield(L, -2, "Semicolon");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Equal));
        lua_setfield(L, -2, "Equal");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::A));
        lua_setfield(L, -2, "A");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::B));
        lua_setfield(L, -2, "B");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::C));
        lua_setfield(L, -2, "C");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::D));
        lua_setfield(L, -2, "D");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::E));
        lua_setfield(L, -2, "E");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F));
        lua_setfield(L, -2, "F");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::G));
        lua_setfield(L, -2, "G");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::H));
        lua_setfield(L, -2, "H");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::I));
        lua_setfield(L, -2, "I");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::J));
        lua_setfield(L, -2, "J");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::K));
        lua_setfield(L, -2, "K");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::L));
        lua_setfield(L, -2, "L");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::M));
        lua_setfield(L, -2, "M");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::N));
        lua_setfield(L, -2, "N");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::O));
        lua_setfield(L, -2, "O");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::P));
        lua_setfield(L, -2, "P");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Q));
        lua_setfield(L, -2, "Q");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::R));
        lua_setfield(L, -2, "R");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::S));
        lua_setfield(L, -2, "S");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::T));
        lua_setfield(L, -2, "T");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::U));
        lua_setfield(L, -2, "U");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::V));
        lua_setfield(L, -2, "V");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::W));
        lua_setfield(L, -2, "W");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::X));
        lua_setfield(L, -2, "X");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Y));
        lua_setfield(L, -2, "Y");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Z));
        lua_setfield(L, -2, "Z");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftBracket));
        lua_setfield(L, -2, "LeftBracket");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Backslash));
        lua_setfield(L, -2, "Backslash");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightBracket));
        lua_setfield(L, -2, "RightBracket");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::GraveAccent));
        lua_setfield(L, -2, "GraveAccent");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::World1));
        lua_setfield(L, -2, "World1");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::World2));
        lua_setfield(L, -2, "World2");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Escape));
        lua_setfield(L, -2, "Escape");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Enter));
        lua_setfield(L, -2, "Enter");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Tab));
        lua_setfield(L, -2, "Tab");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Backspace));
        lua_setfield(L, -2, "Backspace");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Insert));
        lua_setfield(L, -2, "Insert");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Delete));
        lua_setfield(L, -2, "Delete");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Right));
        lua_setfield(L, -2, "Right");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Left));
        lua_setfield(L, -2, "Left");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Down));
        lua_setfield(L, -2, "Down");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Up));
        lua_setfield(L, -2, "Up");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::PageUp));
        lua_setfield(L, -2, "PageUp");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::PageDown));
        lua_setfield(L, -2, "PageDown");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Home));
        lua_setfield(L, -2, "Home");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::End));
        lua_setfield(L, -2, "End");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::CapsLock));
        lua_setfield(L, -2, "CapsLock");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::ScrollLock));
        lua_setfield(L, -2, "ScrollLock");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumLock));
        lua_setfield(L, -2, "NumLock");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::PrintScreen));
        lua_setfield(L, -2, "PrintScreen");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Pause));
        lua_setfield(L, -2, "Pause");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F1));
        lua_setfield(L, -2, "F1");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F2));
        lua_setfield(L, -2, "F2");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F3));
        lua_setfield(L, -2, "F3");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F4));
        lua_setfield(L, -2, "F4");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F5));
        lua_setfield(L, -2, "F5");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F6));
        lua_setfield(L, -2, "F6");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F7));
        lua_setfield(L, -2, "F7");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F8));
        lua_setfield(L, -2, "F8");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F9));
        lua_setfield(L, -2, "F9");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F10));
        lua_setfield(L, -2, "F10");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F11));
        lua_setfield(L, -2, "F11");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F12));
        lua_setfield(L, -2, "F12");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F13));
        lua_setfield(L, -2, "F13");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F14));
        lua_setfield(L, -2, "F14");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F15));
        lua_setfield(L, -2, "F15");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F16));
        lua_setfield(L, -2, "F16");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F17));
        lua_setfield(L, -2, "F17");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F18));
        lua_setfield(L, -2, "F18");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F19));
        lua_setfield(L, -2, "F19");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F20));
        lua_setfield(L, -2, "F20");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F21));
        lua_setfield(L, -2, "F21");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F22));
        lua_setfield(L, -2, "F22");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F23));
        lua_setfield(L, -2, "F23");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F24));
        lua_setfield(L, -2, "F24");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::F25));
        lua_setfield(L, -2, "F25");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadZero));
        lua_setfield(L, -2, "NumpadZero");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadOne));
        lua_setfield(L, -2, "NumpadOne");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadTwo));
        lua_setfield(L, -2, "NumpadTwo");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadThree));
        lua_setfield(L, -2, "NumpadThree");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadFour));
        lua_setfield(L, -2, "NumpadFour");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadFive));
        lua_setfield(L, -2, "NumpadFive");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadSix));
        lua_setfield(L, -2, "NumpadSix");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadSeven));
        lua_setfield(L, -2, "NumpadSeven");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadEight));
        lua_setfield(L, -2, "NumpadEight");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadNine));
        lua_setfield(L, -2, "NumpadNine");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadDecimal));
        lua_setfield(L, -2, "NumpadDecimal");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadDivide));
        lua_setfield(L, -2, "NumpadDivide");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadMultiply));
        lua_setfield(L, -2, "NumpadMultiply");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadSubtract));
        lua_setfield(L, -2, "NumpadSubtract");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadAdd));
        lua_setfield(L, -2, "NumpadAdd");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadEnter));
        lua_setfield(L, -2, "NumpadEnter");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::NumpadEqual));
        lua_setfield(L, -2, "NumpadEqual");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftShift));
        lua_setfield(L, -2, "LeftShift");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftControl));
        lua_setfield(L, -2, "LeftControl");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftAlt));
        lua_setfield(L, -2, "LeftAlt");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::LeftSuper));
        lua_setfield(L, -2, "LeftSuper");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightShift));
        lua_setfield(L, -2, "RightShift");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightControl));
        lua_setfield(L, -2, "RightControl");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightAlt));
        lua_setfield(L, -2, "RightAlt");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::RightSuper));
        lua_setfield(L, -2, "RightSuper");
        lua_pushnumber(L, static_cast<LUA_NUMBER>(KeyEnum::Menu));
        lua_setfield(L, -2, "Menu");

        lua_setfield(L, -2, "KeyEnum"); // set BIGGEngine.KeyEnum
    }

    void open_ActionEnum(lua_State* L) {
        lua_createtable(L, 0, 3);      // create table ActionEnum

        lua_pushnumber(L, static_cast<lua_Number>(ActionEnum::Release));
        lua_setfield(L, -2, "Release");
        lua_pushnumber(L, static_cast<lua_Number>(ActionEnum::Press));
        lua_setfield(L, -2, "Press");
        lua_pushnumber(L, static_cast<lua_Number>(ActionEnum::Repeat));
        lua_setfield(L, -2, "Repeat");

        lua_setfield(L, -2, "ActionEnum"); // set BIGGEngine.ActionEnum

    }

    void open_ModsEnum(lua_State* L) {
        lua_createtable(L, 0, 6);      // create table ModsEnum

        lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Shift));
        lua_setfield(L, -2, "Shift");
        lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Control));
        lua_setfield(L, -2, "Control");
        lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Alt));
        lua_setfield(L, -2, "Alt");
        lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::Super));
        lua_setfield(L, -2, "Super");
        lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::CapsLock));
        lua_setfield(L, -2, "CapsLock");
        lua_pushnumber(L, static_cast<lua_Number>(ModsEnum::NumLock));
        lua_setfield(L, -2, "NumLock");

        lua_setfield(L, -2, "ModsEnum"); // set BIGGEngine.ModsEnum

    }

    using vec2h = glm::vec2;
    using ivec2h = glm::ivec2;
    using vec3h = glm::vec3;
    using ivec3h = glm::ivec3;

    template <typename T>
    const char* g_vecMTName;
    template <> const char* g_vecMTName< vec2h> = "BIGGEngine.Vec2Handle";
    template <> const char* g_vecMTName<ivec2h> = "BIGGEngine.IVec2Handle";
    template <> const char* g_vecMTName< vec3h> = "BIGGEngine.Vec3Handle";
    template <> const char* g_vecMTName<ivec3h> = "BIGGEngine.IVec3Handle";

    template <typename T>
    const char* g_vecIndexFuncName;
    template <> const char* g_vecIndexFuncName< vec2h> =  "Vec2HandleIndex";
    template <> const char* g_vecIndexFuncName<ivec2h> = "IVec2HandleIndex";
    template <> const char* g_vecIndexFuncName< vec3h> =  "Vec3HandleIndex";
    template <> const char* g_vecIndexFuncName<ivec3h> = "IVec3HandleIndex";

    template <typename T>
    const char* g_vecNewIndexFuncName;
    template <> const char* g_vecNewIndexFuncName< vec2h> =  "Vec2HandleNewIndex";
    template <> const char* g_vecNewIndexFuncName<ivec2h> = "IVec2HandleNewIndex";
    template <> const char* g_vecNewIndexFuncName< vec3h> =  "Vec3HandleNewIndex";
    template <> const char* g_vecNewIndexFuncName<ivec3h> = "IVec3HandleNewIndex";

    template <typename T>
    const char* g_vecToStringFuncName;
    template <> const char* g_vecToStringFuncName< vec2h> =  "Vec2HandleToString";
    template <> const char* g_vecToStringFuncName<ivec2h> = "IVec2HandleToString";
    template <> const char* g_vecToStringFuncName< vec3h> =  "Vec3HandleToString";
    template <> const char* g_vecToStringFuncName<ivec3h> = "IVec3HandleToString";

    const char* g_EntityMTName = "BIGGEngine.Entity";
    const char* g_EntityIndexFuncName = "EntityIndex";
    const char* g_EntityToStringFuncName = "EntityToString";

    const char* g_TransformComponentMTName = "BIGGEngine.TransformComponent";
    const char* g_TransformComponentIndexFuncName = "TransformComponentIndex";
    const char* g_TransformComponentNewIndexFuncName = "TransformComponentNewIndex";
    const char* g_TransformComponentToStringFuncName = "TransformComponentToString";


    /// Leaves lua Stack unchanged.
    /// May throw a lua error if the first argument is not a vector handle.
    /// @returns a vector pointer.
    template <typename VecT>
    VecT* getVectorHandleArg(lua_State* L) {

        void* userdata = luaL_checkudata(L, 1, g_vecMTName<VecT>);
        luaL_argcheck(L, userdata != nullptr, 1, "vector handle expected");

        lua_getiuservalue(L, 1, 1); // pushes entity onto stack
        auto* pVec = static_cast<VecT*>(lua_touserdata(L, -1));
        lua_pop(L, 1);

        return pVec;
    }

    /// Leaves lua Stack unchanged.
    /// May throw a lua error with message @p errorMsg if the @p argIndex argument is not a valid lua string.
    const char* getStringArg(lua_State* L, int argIndex, const char* errorMsg = "valid string expected") {
        const char* s = luaL_checkstring(L, argIndex);
        luaL_argcheck(L, s != nullptr, argIndex, errorMsg);
        return s;
    }

    /// May throw a lua error if the first argument is not a vector handle or if second argument
    /// is an invalid index.
    template <typename VecT>
    int l_vectorHandleIndex(lua_State* L) {
        const VecT* pVec = getVectorHandleArg<VecT>(L);
        lua_Integer index = luaL_checkinteger(L, 2);

        luaL_argcheck(L, index > 0 && index <= VecT::length(), 2, "invalid index");

        lua_pushnumber(L, pVec->operator[](index-1)); // index-1 since lua indices are one higher than c indices.
        return 1;
    }

    /// May throw a lua error if the first argument is not a vector handle or if second argument
    /// is an invalid index.
    template <typename VecT>
    int l_vectorHandleNewIndex(lua_State* L) {
        VecT* pVec  = getVectorHandleArg<VecT>(L);
        lua_Integer index = luaL_checkinteger(L, 2);
        lua_Number value  = luaL_checknumber(L, 3);

        luaL_argcheck(L, index > 0 && index <= VecT::length(), 2, "invalid index");

        pVec->operator[](index-1) = value;
        return 0;
    }

    template<typename VecT>
    int l_vectorHandleToString(lua_State* L) {
        VecT* pVec = getVectorHandleArg<VecT>(L);

        std::string s;
        if(VecT::length() == 2) {
            s = fmt::format("({:.3f}, {:.3f})", pVec->x, pVec->y);
        } else if(VecT::length() == 3) {
            s = fmt::format("({:.3f}, {:.3f}, {:.3f})", pVec->x, pVec->y, (*pVec)[2]);
        }
        lua_pushstring(L, s.c_str());
        return 1;
    }


    /// Pushes a BIGGEngine.VectorHandle<VecT> onto the stack.
    template <typename VecT>
    void newVectorHandle(lua_State* L, VecT* pVec){
        if(pVec == nullptr) {
            // create a new userdata which is really an array. set its user value to itself
            // so other functions work the same.
            auto* userdata = static_cast<VecT*>(lua_newuserdatauv(L, sizeof(typename VecT::value_type) * VecT::length(), 1));
            (*userdata) = VecT(0); // init it to 0.
            lua_pushlightuserdata(L, userdata);
            lua_setiuservalue(L, -2, 1);
        } else {
            lua_newuserdatauv(L, 0, 1);
            lua_pushlightuserdata(L, pVec);
            lua_setiuservalue(L, -2, 1);
        }

        luaL_setmetatable(L, g_vecMTName<VecT>);
    }

    /// Registers metatable BIGGEngine.[x]Vector[y]Handle. Leaves lua stack unchanged.
    /// where [x] is optionally @p I for integer and [y] is @p 2 or @p 3.
    /// Expects a stack with table 'BIGGEngine' at the top.
    template<typename VecT>
    void open_VectorHandle(lua_State* L) {
        luaL_newmetatable(L, g_vecMTName<VecT>);
        lua_getfield(L,-2, g_vecIndexFuncName<VecT>); // push function BIGGEngine.VecIndex to top
        lua_setfield(L, -2, "__index"); // set mt.__index = BIGGEngine.VecIndex

        lua_getfield(L, -2, g_vecNewIndexFuncName<VecT>);
        lua_setfield(L, -2, "__newindex");

        lua_getfield(L, -2, g_vecToStringFuncName<VecT>);
        lua_setfield(L, -2, "__tostring");

        lua_pop(L, 1); // pop metatable Vec3Handle
    }

    /// Pushes a BIGGEngine.TransformComponent onto the stack.
    void newTransformComponent(lua_State* L, Transform* pTransform){
        void* userdata = lua_newuserdatauv(L, 0, 1);
        lua_pushlightuserdata(L, pTransform);
        lua_setiuservalue(L, -2, 1);

        luaL_setmetatable(L, g_TransformComponentMTName);
    }

    int l_TransformComponentIndex(lua_State* L) {
        void* userdata = luaL_checkudata(L, 1, g_TransformComponentMTName);
        const char* key = luaL_checkstring(L, 2);

        luaL_argcheck(L, userdata != nullptr, 1, "transform component expected");

        bool isPos = strcmp("position", key) == 0;
        bool isRot = strcmp("rotation", key) == 0;
        bool isScale = strcmp("scale", key) == 0;
        luaL_argcheck(L, isPos || isRot || isScale, 2, "'position' or 'rotation' or 'scale' expected");

        lua_getiuservalue(L, 1, 1); // pushes entity onto stack
        auto* pTransform = static_cast<Transform*>(lua_touserdata(L, -1));
        lua_pop(L, 1);

        if(isPos) {
            newVectorHandle<vec3h>(L, &pTransform->position);
        } else if(isRot) {
            newVectorHandle<vec3h>(L, &pTransform->rotation);
        } {
            newVectorHandle<vec3h>(L, &pTransform->scale);
        }
        return 1;
    }

    int l_TransformComponentNewIndex(lua_State* L) {
        /// take in a table with members x, y, z. or [0], [1], [2]
        return 0;
    }

    /// Expects a stack with table 'BIGGEngine' at the top.
    void open_TransformComponent(lua_State* L) {
        luaL_newmetatable(L, g_TransformComponentMTName);
        // set __index
        lua_getfield(L, -2, g_TransformComponentIndexFuncName);
        lua_setfield(L, -2, "__index");
        // set __newindex
        lua_getfield(L, -2, g_TransformComponentNewIndexFuncName);
        lua_setfield(L, -2, "__newindex");

        lua_pop(L, 1);
    }

    /// Pushes a BIGGEngine.TransformComponent onto the stack.
    void l_newEntity(lua_State* L, entt::entity entity){
        void* userdata = (lua_newuserdatauv(L, 0, 1));
        lua_pushinteger(L, static_cast<lua_Integer>(entity));
        lua_setiuservalue(L, -2, 1);

        luaL_setmetatable(L, g_EntityMTName);
    }

    int l_EntityIndex(lua_State* L) {
        void* userdata = (luaL_checkudata(L, 1, g_EntityMTName));
        const char* key = luaL_checkstring(L, 2);

        luaL_argcheck(L, userdata != nullptr, 1, "entity expected");

        bool isTransform = strcmp("transform", key) == 0;
        luaL_argcheck(L, isTransform, 2, "'transform' expected");

        lua_getiuservalue(L, 1, 1); // pushes entity onto stack
        entt::entity entity = static_cast<entt::entity>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        if(isTransform) {
            newTransformComponent(L, &ECS::get().get<Transform>(entity));
        }
        return 1;
    }

    /// Expects a stack with table 'BIGGEngine' at the top.
    void open_Entity(lua_State* L) {
        luaL_newmetatable(L, g_EntityMTName);
        // set __index
        lua_getfield(L, -2, g_EntityIndexFuncName);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);
    }

    // ------------ Lua Vector Functions ----------------------
#include "../scripts/LuaVector.hpp"
#include "../scripts/LuaVector-inl.hpp"

#include "../scripts/LuaContextFunctions.hpp"
#include "../scripts/LuaContextFunctions-inl.hpp"

    // ------------ misc lua functions -------------------------

    int l_log(lua_State *L) {
        int numArgs = lua_gettop(L);

        for(int i = 0; i < numArgs; i++) {
            luaL_argcheck(L, !lua_isnil(L, i+1), i+1, "valid type expected, got nil");
        }

        lua_getglobal(L, "string");     // library 'string'
        lua_getfield(L, -1, "format");  // function 'format'
        lua_insert(L, 1);               // insert function 'string.format' to bottom of stack
        lua_pop(L, 1);                  // pop table 'string' from stack
        // now stack looks like:
        //      function 'string.format'
        //      arg 1
        //      ...
        //      arg n
        if(lua_pcall(L, numArgs, 1, 0)) {
            lua_error(L);
        }
        const char* msg = luaL_checkstring(L, -1);
        lua_pop(L, 1);

        spdlog::source_loc sourceLoc;
        lua_Debug debug;
        if (lua_getstack(L, 1, &debug)) {  // check function at level 1
            lua_getinfo(L, "lnS", &debug);  // get info about it

            sourceLoc.filename = debug.short_src;
            sourceLoc.line = debug.currentline;
            sourceLoc.funcname = (debug.name)?debug.name:SPDLOG_FUNCTION;

        } else {
            BIGG_LOG_WARN("lua_getstack() failed.");
        }

        Log::m_scriptLogger->log(sourceLoc, spdlog::level::level_enum::info, msg);
        return 0;
    }

    const luaL_Reg BIGGEnginelib[] = {

            // Misc funcs
            {"log", l_log},

            // Vector Handle Funcs
            {g_vecIndexFuncName< vec2h>, l_vectorHandleIndex< vec2h>},
            {g_vecIndexFuncName<ivec2h>, l_vectorHandleIndex<ivec2h>},
            {g_vecIndexFuncName< vec3h>, l_vectorHandleIndex< vec3h>},
            {g_vecIndexFuncName<ivec3h>, l_vectorHandleIndex<ivec3h>},
            {g_vecNewIndexFuncName< vec2h>, l_vectorHandleNewIndex< vec2h>},
            {g_vecNewIndexFuncName<ivec2h>, l_vectorHandleNewIndex<ivec2h>},
            {g_vecNewIndexFuncName< vec3h>, l_vectorHandleNewIndex< vec3h>},
            {g_vecNewIndexFuncName<ivec3h>, l_vectorHandleNewIndex<ivec3h>},
            {g_vecToStringFuncName< vec2h>, l_vectorHandleToString< vec2h>},
            {g_vecToStringFuncName<ivec2h>, l_vectorHandleToString<ivec2h>},
            {g_vecToStringFuncName< vec3h>, l_vectorHandleToString< vec3h>},
            {g_vecToStringFuncName<ivec3h>, l_vectorHandleToString<ivec3h>},

            // Vector Funcs
            {g_VectorIndexFuncName, l_VectorIndex},
            {g_VectorNewIndexFuncName, l_VectorNewIndex},
            {g_VectorToStringFuncName, l_VectorToString},

            // Context Funcs
            {g_GetWindowSizeFuncName, l_getWindowSize},
            {g_GetWindowFramebufferSizeFuncName, l_getWindowFramebufferSize},
            {g_GetWindowContentScaleFuncName, l_getWindowContentScale},
            {g_GetWindowPositionFuncName, l_getWindowPosition},
            {g_GetWindowIconifiedFuncName, l_getWindowIconified},
            {g_GetWindowMaximizedFuncName, l_getWindowMaximized},
            {g_GetWindowVisibleFuncName, l_getWindowVisible},
            {g_GetWindowFocusFuncName, l_getWindowFocus},
            {g_GetKeyFuncName, l_getKey},
            {g_GetMousePositionFuncName, l_getMousePosition},
            {g_GetMouseHoverFuncName, l_getMouseHover},
            {g_GetMouseButtonFuncName, l_getMouseButton},
            {g_GetClipboardStringFuncName, l_getClipboardString},
            {g_SetWindowShouldCloseFuncName, l_setWindowShouldClose},
            {g_SetWindowSizeLimitsFuncName, l_setWindowSizeLimits},
            {g_SetWindowAspectRatioFuncName, l_setWindowAspectRatio},
            {g_SetWindowTitleFuncName, l_setWindowTitle},
            {g_SetClipboardStringFuncName, l_setClipboardString},
            {g_SetWindowVisibleFuncName, l_setWindowVisible},

            // Component funcs
            {g_TransformComponentIndexFuncName, l_TransformComponentIndex},
            {g_TransformComponentNewIndexFuncName, l_TransformComponentNewIndex},
            {g_EntityIndexFuncName, l_EntityIndex},
            {nullptr, nullptr},
    };

    int open_BIGGEnginelib(lua_State *L) {
        luaL_newlib(L, BIGGEnginelib);            // create table BIGGEngine
        open_KeyEnum(L);
        open_ActionEnum(L);
        open_ModsEnum(L);
        open_MouseButtonEnum(L);

        open_VectorHandle< vec2h>(L);
        open_VectorHandle<ivec2h>(L);
        open_VectorHandle< vec3h>(L);
        open_VectorHandle<ivec3h>(L);

        open_TransformComponent(L);
        open_Entity(L);
        open_VectorMT(L);

        return 1;
    }

    /// Pushes onto the stack a copy a lua table. Ignores metatables. Does not work with nested tables.
    /// Only copies non-default data (ie. default libs, functions, etc.)
    /// @param srcIndex is the stack index of the source lua table
    void copyTableShallow(lua_State* L, int srcIndex) {
        if(!lua_istable(L, srcIndex)) {
            // any POD is copied by value. any userdata, threads, or functions are copied by reference.
            lua_pushvalue(L, srcIndex);
            return;
        }
        srcIndex = lua_absindex(L, srcIndex);
        lua_newtable(L);
        int outputIndex = lua_absindex(L, -1);

        lua_pushnil(L);  // first key
        while (lua_next(L, srcIndex) != 0) {
            if (lua_isstring(L, -2)) {
                if (isDefaultName(luaL_checkstring(L, -2))) {
                    lua_pop(L, 1);
                    continue;
                }
            }
            lua_pushvalue(L, -2);     // stack looks like -3 = key, -2 = value, -1 = key
            lua_insert(L, -2);        // stack looks like -3 = key, -2 = key, -1 = value
            lua_rawset(L, outputIndex); // removes 2 objects, leaving 1 key for next call to lua_next().
        }
    }

    /// set key-value pairs from table at @p srcIndex to table at @p destIndex.
    /// any key-value pairs from @p destIndex not in @p src index are ignored.
    void loadTableShallow(lua_State* L, int srcIndex, int destIndex) {
        if(!lua_istable(L, srcIndex) || !lua_istable(L, destIndex)) {
            // any POD is copied by value. any userdata, threads, or functions are copied by reference.
            return;
        }
        srcIndex = lua_absindex(L, srcIndex);
        destIndex = lua_absindex(L, destIndex);
        lua_pushnil(L);  // first key
        while (lua_next(L, srcIndex) != 0) {
            lua_pushvalue(L, -2);     // stack looks like -3 = key, -2 = value, -1 = key
            lua_insert(L, -2);        // stack looks like -3 = key, -2 = key, -1 = value
            lua_rawset(L, destIndex); // removes 2 objects, leaving 1 key for next call to lua_next().
        }
    }

    /// prints a lua table at stack position @p tableIndex to std::out
    /// @param tableIdx is the stack index of the table to print
    /// @param _depth is for internal use only
    void printTable(lua_State* L, int tableIndex, int _depth = 0) {
        tableIndex = lua_absindex(L, tableIndex);
        lua_pushnil(L);  // first key
        while (lua_next(L, tableIndex) != 0) {
            // uses 'key' (at index -2) and 'value' (at index -1)
            const char *key;
            if (lua_isnumber(L, -2)) {
                lua_Number numKey = lua_tonumber(L, -2);
                key = std::to_string(numKey).c_str();
            } else {
                key = lua_tostring(L, -2);
            }
            int valueType = lua_type(L, -1);
            if (lua_isnil(L, -1)) {
                BIGG_LOG_INFO("{} nil {} = nil", std::string(_depth * 4, ' '), key);
            } else if (lua_isinteger(L, -1)) {
                BIGG_LOG_INFO("{} integer {} = {}", std::string(_depth * 4, ' '), key, lua_tointeger(L, -1));
            } else if (lua_isnumber(L, -1)) {
                BIGG_LOG_INFO("{} double {} = {}", std::string(_depth * 4, ' '), key, lua_tonumber(L, -1));
            } else if (lua_isboolean(L, -1)) {
                BIGG_LOG_INFO("{} bool {} = {}", std::string(_depth*4, ' '), key, (lua_toboolean(L, -1)?"true":"false"));
            } else if (lua_isstring(L, -1)) {
                BIGG_LOG_INFO("{} string {} = {}", std::string(_depth*4, ' '), key, lua_tostring(L, -1));
            } else if (lua_istable(L, -1)) {

                BIGG_LOG_INFO("{} table {} {{", std::string(_depth*4, ' '), key);
                if(_depth < 1) printTable(L, -1, _depth + 1);
                BIGG_LOG_INFO("{} }}", std::string(_depth*4, ' '));
            } else if (lua_isfunction(L, -1)) {
                BIGG_LOG_INFO("{} function {} = ???", std::string(_depth*4, ' '), key);
            } else if (lua_isuserdata(L, -1)) {
                BIGG_LOG_INFO("{} userdata {} = {}", std::string(_depth*4, ' '), key, lua_touserdata(L, -1));
            } else if (lua_isthread(L, -1)) {
                BIGG_LOG_INFO("{} thread {} = {}", std::string(_depth*4, ' '), key, (void*)lua_tothread(L, -1));
            } else if (lua_islightuserdata(L, -1)) {
                BIGG_LOG_INFO("{} light userdata {} = {}", std::string(_depth*4, ' '), key, lua_touserdata(L, -1));
            } else {
                BIGG_LOG_INFO("{} unknown type {} = ???", std::string(_depth*4, ' '), key);
            }
            lua_pop(L, 1); // pop the value and leave the key for next lua_next() call
        }
    }

    // Script Handle implementation

    ScriptHandle::ScriptHandle(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority)
            : name(name.value()) {

        BIGG_PROFILE_SCRIPT_FUNCTION;

        {
        BIGG_PROFILE_SCRIPT_SCOPE("setup member vars '{}'", name.data());

        m_luaState = luaL_newstate();
        luaL_openlibs(m_luaState);
        luaL_requiref(m_luaState, "BIGGEngine", open_BIGGEnginelib, 1);
        // If glb is true, also stores the module into the global modname. aka global 'BIGGEngine'
//            luaL_requiref(L, "BIGGEngine", open_mylib, 1);
        lua_pop(m_luaState, 1); // pop the requiref.

        auto warnFunc = [](void *, const char *msg, int) {
            BIGG_LOG_WARN(msg);
        };

        lua_setwarnf(m_luaState, static_cast<lua_WarnFunction>(warnFunc), nullptr);

        // load file. Pushes the chunk as a Lua function onto the stack.
        // TODO filepath.data() may not be null terminated
        CHECK_LUA(m_luaState, luaL_loadfile(m_luaState, filepath.data()));

        // Does NOT pop the Lua function off the stack.
//        if (lua_dump(m_luaState, writer, &m_buffer, false)) {
//            lua_error(m_luaState);
//        }
//        BIGG_LOG_DEBUG("Cached script '{}' path: '{}' size: {} bytes.", name, filepath, m_buffer.size());
        } // BIGG_PROFILE_SCRIPT_SCOPE("setup member vars {}", name.data());


        BIGG_PROFILE_SCRIPT_SCOPE("set callbacks '{}'", name.data());   // rest of function

        lua_getglobal(m_luaState, "BIGGEngine"); // push _ENV.BIGGEngine to stack

        lua_pushstring(m_luaState, name.data());           // push new name
        lua_setfield(m_luaState, -2, "ScriptName"); // pops name from stack, but keeps table BIGGEngine

        lua_pop(m_luaState, 1);                         // pop table _ENV.BIGGEngine

        CHECK_LUA(m_luaState, lua_pcall(m_luaState, 0, LUA_MULTRET, 0)); // call chunk

        lua_getglobal(m_luaState, "BIGGEngine");
        lua_getfield(m_luaState, -1, "Init");
        if (!lua_isfunction(m_luaState, -1)) {
            BIGG_LOG_WARN("no Init function detected for script '{}'.", name);
            lua_pop(m_luaState, 2); // pop BIGGEngine, Init
            return;
        }

        CHECK_LUA(m_luaState, lua_pcall(m_luaState, 0, 0, 0)); // call Init

        lua_getfield(m_luaState, -1, "MouseButton");
        if (lua_isfunction(m_luaState, -1)) {
            // setup callback
            auto callback = [=](MouseButtonEvent event) -> bool {   // captures this.m_luaState and name
                _BIGG_PROFILE_CATEGORY_SCOPE("script", "callback");

                lua_getglobal(m_luaState, "BIGGEngine");
                BIGG_ASSERT(lua_istable(m_luaState, -1), "BIGGEngine is not a table! '{}'", name.data());

                // for each ScriptComponent
                auto& registry = ECS::get();
                auto view = registry.view<ScriptComponent>();
                for(auto [entity, scriptComponent] : view.each()) {

                    // Omit all scripts with a different name
                    if(scriptComponent.m_name != name.value()) continue;

                    // load 'this' entity handle userdata
                    l_newEntity(m_luaState, entity);
                    lua_setglobal(m_luaState,"this");

                    // load saved globals
                    lua_rawgetp(m_luaState, LUA_REGISTRYINDEX, (void*)&scriptComponent);
                    lua_rawgeti(m_luaState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
                    loadTableShallow(m_luaState, -2, -1);
                    lua_pop(m_luaState, 2);

                    // get function callback
                    lua_getfield(m_luaState, -1, "MouseButton");
                    BIGG_ASSERT(lua_isfunction(m_luaState, -1), "MouseButton is not a function! '{}'", name.data());

                    // push args
                    lua_pushinteger(m_luaState, static_cast<lua_Integer>(event.m_button));
                    lua_pushinteger(m_luaState, static_cast<lua_Integer>(event.m_action));
                    lua_pushinteger(m_luaState, static_cast<lua_Integer>(event.m_mods));
                    // call lua function
                    if(lua_pcall(m_luaState, 3, 1, 0)) { // pops args and function from stack
                        lua_error(m_luaState);
                        return false;
                    }
                    // get return value
                    if (!lua_isboolean(m_luaState, -1)) {
                        // error! should have returned a bool!
                        BIGG_LOG_WARN("lua BIGGEngine.MouseButton didn't return a bool!");
                        lua_pop(m_luaState, 1);
                        return false;
                    }
                    bool ret = lua_toboolean(m_luaState, -1);
                    lua_pop(m_luaState, 1); // pop the return value from the stack

                    // save globals
                    lua_pushglobaltable(m_luaState);
                    copyTableShallow(m_luaState, -1);
                    lua_rawsetp(m_luaState, LUA_REGISTRYINDEX, (void*)&scriptComponent); // key is light userdata of this ScriptComponent
                    lua_pop(m_luaState, 1);

//                    if(event.m_action == BIGGEngine::ActionEnum::Press) {
//                        lua_rawgetp(m_luaState, LUA_REGISTRYINDEX, (void *) &scriptComponent);
//                        printTable(m_luaState, -1);
//                        lua_pop(m_luaState, 1);
//                    }

                    if(ret) return true;    // only the first instance of this script will be ran.
                }

                lua_pop(m_luaState, 1); // pop table BIGGEngine
                return false;
            };
            Events::subscribe<MouseButtonEvent>(subscribePriority, callback);

            lua_pop(m_luaState, 1); // pop MouseButton
        }

        // TODO repeat for all other callbacks
        lua_pop(m_luaState, 1); // pop BIGGEngine
    }

    ScriptHandle::~ScriptHandle() {
        BIGG_PROFILE_SHUTDOWN_FUNCTION;
        lua_close(m_luaState);
    }

} // anonymous namespace

 // --------------------------- Public things --------------------------------

    /// registers a script's name, loads, caches, and runs the script.
    void registerScript(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority) {
        // If a key equivalent to k already exists in the container, does nothing.
        // calls constructor of ScriptHandle
        g_registeredScripts.try_emplace(name.value(), name, filepath, subscribePriority);
    }
} // namepace Scripting

    ScriptComponent::ScriptComponent(entt::hashed_string::hash_type name) : m_name(name) {
        // save globals
        using namespace Scripting;
        if(g_registeredScripts.count(m_name) == 0) {
            return;
        }
        const ScriptHandle& handle = g_registeredScripts.at(m_name);
        lua_State* L = handle.m_luaState;

        lua_pushglobaltable(L);
        copyTableShallow(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)this); // key is light userdata of this ScriptComponent
        lua_pop(L, 1);
    }

    ScriptComponent::~ScriptComponent() {
        using namespace Scripting;
        if(g_registeredScripts.count(m_name) == 0) {
            return;
        }
        const ScriptHandle& handle = g_registeredScripts.at(m_name);
        lua_State* L = handle.m_luaState;

        lua_pushnil(L);
        lua_rawsetp(L, LUA_REGISTRYINDEX, (void*)this);

    }

} // namespace BIGGEngine {
