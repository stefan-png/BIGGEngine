//
// Created by Stefan Antoszko on 2022-05-27.
//
#include "Script.hpp"

#define BIGG_PROFILE_SCRIPT_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("script")
#define BIGG_PROFILE_SCRIPT_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("script", _format, ##__VA_ARGS__)

#include <utility> // for helper functions

#include <string> // for debugging log messages

namespace BIGGEngine {
namespace Scripting {
namespace {

    // forward declarations
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

    // all the open_XxxEnum functions expect a stack with table 'BIGGEngine' at the top.
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
            {"log", l_log},
            {nullptr, nullptr},
    };

    int open_BIGGEnginelib(lua_State *L) {
        luaL_newlib(L, BIGGEnginelib);            // create table BIGGEngine
        open_KeyEnum(L);
        open_ActionEnum(L);
        open_ModsEnum(L);
        open_MouseButtonEnum(L);
        return 1;
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
            BIGG_LOG_WARN("{}", msg);
        };

        lua_setwarnf(m_luaState, static_cast<lua_WarnFunction>(warnFunc), nullptr);

        // load file. Pushes the chunk as a Lua function onto the stack.
        // TODO filepath.data() may not be null terminated
        if (luaL_loadfile(m_luaState, filepath.data())) {
            lua_error(m_luaState);
        }
        // Does NOT pop the Lua function off the stack.
        if (lua_dump(m_luaState, writer, &m_buffer, false)) {
            lua_error(m_luaState);
        }
        } // BIGG_PROFILE_SCRIPT_SCOPE("setup member vars {}", name.data());


        BIGG_PROFILE_SCRIPT_SCOPE("set callbacks '{}'", name.data());   // rest of function

        lua_getglobal(m_luaState, "BIGGEngine"); // push _ENV.BIGGEngine to stack

        lua_pushstring(m_luaState, name.data());           // push new name
        lua_setfield(m_luaState, -2, "ScriptName"); // pops name from stack, but keeps table BIGGEngine

        lua_pop(m_luaState, 1);                         // pop table _ENV.BIGGEngine

        // must set the chunk's first upvalue to some environment

        // NOTE: _ENV table from lua_pushglobaltable is still at -1
        lua_setupvalue(m_luaState, -2, 1);  // pops table so chunk is -1
        BIGG_LOG_DEBUG("Cached script '{}' path: '{}' size: {} bytes.", name, filepath, m_buffer.size());

        // NOTE: chunk is at -1
        // this function pops the function and pushes any return values.
        if (lua_pcall(m_luaState, 0, LUA_MULTRET, 0)) {
            lua_error(m_luaState);
        }

        lua_getglobal(m_luaState, "BIGGEngine");
        lua_getfield(m_luaState, -1, "Init");
        if (!lua_isfunction(m_luaState, -1)) {
            BIGG_LOG_WARN("no Init function detected for script '{}'.", name);
            lua_pop(m_luaState, 2); // pop BIGGEngine, Init
            return;
        }
        if (lua_pcall(m_luaState, 0, 0, 0)) {
            lua_error(m_luaState); // pop Init.
        }

        lua_getfield(m_luaState, -1, "MouseButton");
        if (lua_isfunction(m_luaState, -1)) {
            // setup callback
            auto callback = [=](MouseButtonEvent event) -> bool {   // captures this.m_luaState and name
                _BIGG_PROFILE_CATEGORY_SCOPE("script", "callback");

                lua_getglobal(m_luaState, "BIGGEngine");

                // for each ScriptComponent
                auto view = ECS::get().view<ScriptComponent>();
                for(auto [entity, scriptComponent] : view.each()) {

                    if(scriptComponent.name != name.value()) continue;

                    // TODO load persistent data (something like below)
                    // lua_pushinteger(entity);
                    // lua_setglobal(BIGGEngine.this);

                    lua_getfield(m_luaState, -1, "MouseButton");

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
                    if(ret) return true;    // only the first instance of this script will be ran.
                    continue;   // go on to next instance of ScriptComponent
                }

                lua_pop(m_luaState, 1); // pop table BIGGEngine
                return false;
            };
            Events::subscribe<MouseButtonEvent>(subscribePriority, callback);

            lua_pop(m_luaState, 1); // pop MouseButton
            return;
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

    ScriptComponent::ScriptComponent(entt::hashed_string name) : name(name.value()) {}

} // namespace BIGGEngine {
