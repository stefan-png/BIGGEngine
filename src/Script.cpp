//
// Created by Stefan Antoszko on 2022-05-27.
//

#include "Script.hpp"

#define BIGG_PROFILE_SCRIPT_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("script")
#define BIGG_PROFILE_SCRIPT_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("script", _format, ##__VA_ARGS__)


namespace BIGGEngine {

    // ------------------------------------- Buffer ----------------------------
    Scripting::Buffer::Buffer() {}

    Scripting::Buffer::Buffer(void* data, size_t size) {
        BIGG_PROFILE_INIT_FUNCTION;
        if(size <= 0) return;
        m_data = malloc(size);
        memcpy(this->m_data, data, size);
        m_size = size;
    }

    Scripting::Buffer::~Buffer(){
        BIGG_PROFILE_SCRIPT_FUNCTION;
        free(m_data);
    }

    void Scripting::Buffer::push(const void* data, size_t size) {
        BIGG_PROFILE_SCRIPT_FUNCTION;
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
        BIGG_PROFILE_SCRIPT_FUNCTION;
        lua_State* L = Scripting::getL();

        auto warnFunc = [](void* ud, const char* msg, int tocont) {
            BIGG_LOG_WARN("{}", msg);
        };
        lua_setwarnf(L, warnFunc, nullptr);

        if(luaL_dostring(L, "package.path = package.path .. ';../scripts/?.lua'")) {
            lua_error(L);
        }
        if(luaL_dostring(L, "package.cpath = package.cpath .. ';../scripts/?.dyld'")) {
            lua_error(L);
        }
        // require BIGGEngine in each script
        if(luaL_dostring(L, "require(\"BIGGEngine\")")) {
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
        /*
        {
            //   26 basic functions
            // + 9 std libs (coroutine, debug, io, math, os, package, string, table, utf8)
            // + 1 BIGGEngine
            // = 36 global variables.
            lua_createtable(L, 0, 36);

            auto pushBaseLibsIntoTable = [&L](const char *name) {
                lua_pushstring(L, name);
                lua_getglobal(L, name);
                lua_settable(L, -3);
            };
            // basic funcs
            pushBaseLibsIntoTable("_G");
            pushBaseLibsIntoTable("_VERSION");
            pushBaseLibsIntoTable("assert");
            pushBaseLibsIntoTable("collectgarbage");
            pushBaseLibsIntoTable("dofile");
            pushBaseLibsIntoTable("error");
            pushBaseLibsIntoTable("getmetatable");
            pushBaseLibsIntoTable("ipairs");
            pushBaseLibsIntoTable("load");
            pushBaseLibsIntoTable("loadfile");
            pushBaseLibsIntoTable("next");
            pushBaseLibsIntoTable("pairs");
            pushBaseLibsIntoTable("pcall");
            pushBaseLibsIntoTable("print");
            pushBaseLibsIntoTable("rawequal");
            pushBaseLibsIntoTable("rawget");
            pushBaseLibsIntoTable("rawlen");
            pushBaseLibsIntoTable("rawset");
            pushBaseLibsIntoTable("require");
            pushBaseLibsIntoTable("select");
            pushBaseLibsIntoTable("setmetatable");
            pushBaseLibsIntoTable("tonumber");
            pushBaseLibsIntoTable("tostring");
            pushBaseLibsIntoTable("type");
            pushBaseLibsIntoTable("warn");
            pushBaseLibsIntoTable("xpcall");

            // std libs
            pushBaseLibsIntoTable("coroutine");
            pushBaseLibsIntoTable("debug");
            pushBaseLibsIntoTable("io");
            pushBaseLibsIntoTable("math");
            pushBaseLibsIntoTable("os");
            pushBaseLibsIntoTable("package");
            pushBaseLibsIntoTable("string");
            pushBaseLibsIntoTable("table");
            pushBaseLibsIntoTable("utf8");

            pushBaseLibsIntoTable("BIGGEngine");
        }
         */
        // this does the same thing probably.
        lua_pushglobaltable(L);

        // set script-specific data
        // NOTE: _ENV table is at -1
        lua_getfield(L, -1, "BIGGEngine"); // push _ENV.BIGGEngine to stack
//        lua_getglobal(L, "BIGGEngine"); // push _ENV.BIGGEngine to stack

        lua_pushstring(L, name.data());           // push new name
        lua_setfield(L, -2, "ScriptName"); // pops name from stack, but keeps table

        lua_createtable(L, 0, 0);       // reset fields which may have been changed by another script.
        lua_setfield(L, -2, "Callbacks");  // to an empty table.
        lua_createtable(L, 0, 0);
        lua_setfield(L, -2, "Persistent");

        lua_pop(L, 1);                         // pop table BIGGEngine

        // NOTE: _ENV table is still at -1
        lua_setupvalue(L, -2, 1);  // pops table so chunk is -1
        BIGG_LOG_DEBUG("Cached script '{}' path: '{}' size: {} bytes.", name, filepath, m_buffer.size());

        // NOTE: chunk is at -1
        // this function pops the function and pushes any return values.
        if(lua_pcall(L, 0, LUA_MULTRET, 0)) {
            lua_error(L);
        }

        lua_getglobal(L, "BIGGEngine");
        lua_getfield(L, -1, "Callbacks");
        lua_getfield(L, -1, "Init");
        if(!lua_isfunction(L, -1)) {
            BIGG_LOG_WARN("no init function detected for script '{}'.", name);
            lua_pop(L, 3); // pop BIGGEngine, Callbacks, Init
            return;
        }

        if(lua_pcall(L, 0, 0, 0)) {
            lua_error(L); // pop Init.
        }
        lua_pop(L, 2); // pop BIGGEngine, Callbacks.

    }

    int Scripting::ScriptHandle::writer(lua_State* L, const void* p, size_t sz, void * ud) {
        Buffer* buf = static_cast<Buffer*>(ud);
        buf->push(p, sz);
        return 0;
    }

    const char* Scripting::ScriptHandle::reader (lua_State *L, void * ud, size_t *size) {
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
            luaL_requiref(L, "mylib", open_mylib, 1);
        }
        return L;
    }
    void Scripting::registerScript(std::string_view name, std::string_view filepath) {
        // If a key equivalent to k already exists in the container, does nothing.
        // calls constructor of ScriptHandle
        m_registeredScripts.try_emplace(name, name, filepath);
    }

    void Scripting::shutdown() {
        BIGG_PROFILE_SHUTDOWN_FUNCTION;
        lua_close(getL());
    }

    Scripting::Buffer* Scripting::getScriptBuffer(std::string_view name) {
        // .count() Returns the number of elements with key that compares equal to the specified argument key, which is either 1 or 0
        BIGG_ASSERT(m_registeredScripts.count(name), "Lua Script '{1}' is not registered. Call Scripting::registerScript(\"{1}\");", name.data())
        return &m_registeredScripts.at(name).m_buffer;
    }

    ScriptComponent::ScriptComponent(std::string_view name) : m_name(name) {}
} // namespace BIGGEngine {