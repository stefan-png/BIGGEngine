//
// Created by Stefan Antoszko on 2022-05-27.
//

#include "Script.hpp"

namespace BIGGEngine {

    ScriptFunctor::ScriptFunctor(lua_State* L, std::string filename) : m_luaState(L) {
        luaL_openlibs(m_luaState);
        lua_setwarnf(m_luaState, warnFunction, nullptr);

        if(luaL_dostring(m_luaState, "package.path = package.path .. ';../scripts/?.lua'")) {
            lua_error(m_luaState);
        }
        if(luaL_dostring(m_luaState, "package.cpath = package.cpath .. ';../scripts/?.dyld'")) {
            lua_error(m_luaState);
        }
        if(luaL_dofile(m_luaState, filename.c_str())) {
            lua_error(m_luaState);
        }
    }

    bool ScriptFunctor::operator()(BIGGEngine::Event *event) {
        // call lua event callback function
        switch(event->m_type) {
            case Event::EventType::Update:
                break;
            case Event::EventType::Tick:
                break;
            case Event::EventType::WindowCreate:
                break;
            case Event::EventType::WindowDestroy:
                break;
            case Event::EventType::WindowShouldClose:
                break;
            case Event::EventType::WindowSize:
                break;
            case Event::EventType::WindowFramebufferSize:
                break;
            case Event::EventType::WindowContentScale:
                break;
            case Event::EventType::WindowPosition:
                break;
            case Event::EventType::WindowIconify:
                break;
            case Event::EventType::WindowMaximize:
                break;
            case Event::EventType::WindowFocus:
                break;
            case Event::EventType::WindowRefresh:
                break;
            case Event::EventType::Key:
                break;
            case Event::EventType::Char:
                break;
            case Event::EventType::MousePosition:
                break;
            case Event::EventType::MouseEnter:
                break;
            case Event::EventType::MouseButton:
            {
                lua_getglobal(m_luaState, "mouseButtonCb");
                if (lua_isnil(m_luaState, -1) || !lua_isfunction(m_luaState, -1)) {
                    BIGG_LOG_WARN("lua mouseButtonCb is not defined or is not a function!");
                    lua_pop(m_luaState, 1); // pop the nil from lua_getglobal()
                    return false;
                }

                lua_pushnumber(m_luaState, static_cast<int>(static_cast<MouseButtonEvent *>(event)->m_button));
                lua_pushnumber(m_luaState, static_cast<int>(static_cast<MouseButtonEvent *>(event)->m_action));
                lua_pushnumber(m_luaState, static_cast<int>(static_cast<MouseButtonEvent *>(event)->m_mods));

                // TODO change to lua_pcall
                lua_call(m_luaState, 3, 1);
                if (!lua_isboolean(m_luaState, -1)) {
                    // error! should have returned a bool!
                    BIGG_LOG_WARN("lua function 'update' didn't return a bool!");
                    lua_pop(m_luaState, 1);
                    return false;
                }
                bool ret = lua_toboolean(m_luaState, -1);
                lua_pop(m_luaState, 1); // pop the return value from the stack
                return ret;
            }
                break;
            case Event::EventType::Scroll:
                break;
            case Event::EventType::DropPath:
                break;
            default:
                break;
        }
        return false;
    }

    void ScriptFunctor::warnFunction(void* ud, const char* msg, int tocont) {
        BIGG_LOG_WARN("{}", msg);
    }

} // namespace BIGGEngine {