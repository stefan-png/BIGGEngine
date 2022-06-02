//
// Created by Stefan Antoszko on 2022-05-27.
//

#include "Script.hpp"

namespace BIGGEngine {

    // ------------------------------------- Buffer ----------------------------
    Scripting::Buffer::Buffer() {}

    Scripting::Buffer::Buffer(void* data, size_t size) {
        if(size <= 0) return;
        m_data = malloc(size);
        memcpy(this->m_data, data, size);
        m_size = size;
    }

    Scripting::Buffer::~Buffer(){
        free(m_data);
    }

    void Scripting::Buffer::push(const void* data, size_t size) {
        if (size <= 0) return;

        void *d = malloc(m_size + size);
        memcpy((char *) d + m_size, data, size);    // since can't do pointer arithmetic on void*

        memcpy(d, m_data, m_size);  // could copy nothing if m_size == 0
        free(m_data);               // could free nothing if m_data == nullptr

        m_data = d;
        m_size += size;
    }

    // --------------------------------- ScriptHandle ------------------------------------------------

    Scripting::ScriptHandle::ScriptHandle(std::string_view name, std::string_view filepath) : name(name), filepath(filepath) {

        lua_State* L = Scripting::getL();

        auto warnFunc = [](void* ud, const char* msg, int tocont) {
            BIGG_LOG_WARN("{}", msg);
        };
        lua_setwarnf(L, warnFunc, nullptr);

        // set lua global scriptName
        lua_pushstring(L, name.data());
        lua_setglobal(L, "BIGGScriptName");

        if(luaL_dostring(L, "package.path = package.path .. ';../scripts/?.lua'")) {
            lua_error(L);
        }
        if(luaL_dostring(L, "package.cpath = package.cpath .. ';../scripts/?.dyld'")) {
            lua_error(L);
        }

        // load file. Pushes the chunk as a Lua function onto the stack.
        // TODO filepath.data() may not be null terminated
        if(luaL_loadfile(L, filepath.data())) {
            lua_error(L);
        }
        // TODO consider stripping debug info... (set last param strip = false)
        // Does NOT pop the Lua function off the stack.
        if(lua_dump(L, writer, &m_buffer, false)) {
            lua_error(L);
        }

        BIGG_LOG_DEBUG("Cached script {} path: {} size: {} bytes.", name, filepath, m_buffer.size());

        // since the chunk is still on stack, call it.
        // this function pops the function and pushes any return values.
        if(lua_pcall(L, 0, LUA_MULTRET, 0)) {
            lua_error(L);
        }
    }

    int Scripting::ScriptHandle::writer(lua_State* L, const void* p, size_t sz, void* ud) {
        Buffer* buf = static_cast<Buffer*>(ud);
        buf->push(p, sz);
        return 0;
    }

    const char* Scripting::ScriptHandle::reader (lua_State *L, void *ud, size_t *size) {
        Buffer* buf = static_cast<Buffer*>(ud);

        (void)L;  // not used
        *size = buf->size();
        return static_cast<const char*>(buf->data());
    }

    // -------------------------------------- Scripting -----------------------------------
    lua_State * Scripting::getL() {
        if(L == nullptr) {
            L = luaL_newstate();
            luaL_openlibs(L);
        }
        return L;
    }
    void Scripting::registerScript(std::string_view name, std::string_view filepath) {
        // If a key equivalent to k already exists in the container, does nothing.
        // calls constructor of ScriptHandle
        m_registeredScripts.try_emplace(name, name, filepath);
    }

    void Scripting::shutdown() {
        lua_close(getL());
    }

    Scripting::ScriptHandle Scripting::getScript(std::string_view name) {
        // Returns the number of elements with key that compares equal to the specified argument key, which is either 1 or 0
        if(m_registeredScripts.count(name)) {
            return m_registeredScripts.at(name);
        }
        BIGG_LOG_WARN("Lua Script '{1}' is not registered. Call Scripting::registerScript(\"{1}\");", name);
        return {nullptr, nullptr};
    }

    ScriptComponent::ScriptComponent(std::string_view name) : m_name(name) {}
/*

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

*/

} // namespace BIGGEngine {