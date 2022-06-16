//
// Created by Stefan Antoszko on 2022-05-27.
//

#include "Script.hpp"

#define BIGG_PROFILE_SCRIPT_FUNCTION            _BIGG_PROFILE_CATEGORY_FUNCTION("script")
#define BIGG_PROFILE_SCRIPT_SCOPE(_format, ...) _BIGG_PROFILE_CATEGORY_SCOPE("script", _format, ##__VA_ARGS__)

#include <utility> // for helper functions

namespace BIGGEngine {
namespace Scripting {
namespace {

    // forward declarations
    struct Buffer {
        Buffer() {}
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

    private:
        void* m_data = nullptr;
        size_t m_size = 0;
    };

    struct ScriptHandle {
        ScriptHandle(entt::hashed_string name, std::string_view filepath);

        // I'm expecting these to be hardcoded into the executable.
        entt::hashed_string::hash_type name;      // name to be registered

        Buffer m_buffer;            // buffer for caching the script.

        static int writer(lua_State* L, const void* p, size_t sz, void * ud);
        static const char *reader (lua_State *L, void * ud, size_t *size);

        friend struct Buffer;
    };

    // global state
    lua_State* L = nullptr;
    std::unordered_map<entt::hashed_string::hash_type, ScriptHandle> m_registeredScripts;

    lua_State* getLuaState() {
        return L;
    }
    /// returns a ScriptHandle with name and filepath as nullptr if input param name isn't registered.
    Buffer* getScriptBuffer(entt::hashed_string name) {
        // .count() Returns the number of elements with key that compares equal to the specified argument key, which is either 1 or 0
        BIGG_ASSERT(m_registeredScripts.count(name),
                    "Lua Script '{1}' is not registered. Call Scripting::registerScript(\"{1}\");", name.data())
        return &m_registeredScripts.at(name).m_buffer;
    }

    // Script Handle implementation

    ScriptHandle::ScriptHandle(entt::hashed_string name, std::string_view filepath)
            : name(name.value()) {

        BIGG_PROFILE_SCRIPT_FUNCTION;

        auto warnFunc = [](void *, const char *msg, int) {
            BIGG_LOG_WARN("{}", msg);
        };

        lua_setwarnf(L, static_cast<lua_WarnFunction>(warnFunc), nullptr);

        if (luaL_dostring(L, "package.path = package.path .. ';../scripts/?.lua'")) {
            lua_error(L);
        }
        if (luaL_dostring(L, "package.cpath = package.cpath .. ';../scripts/?.dyld'")) {
            lua_error(L);
        }
        // require BIGGEngine in each script
        if (luaL_dostring(L, "require(\"BIGGEngine\")")) {
            lua_error(L);
        }

        // load file. Pushes the chunk as a Lua function onto the stack.
        // TODO filepath.data() may not be null terminated
        if (luaL_loadfile(L, filepath.data())) {
            lua_error(L);
        }
        // Does NOT pop the Lua function off the stack.
        if (lua_dump(L, writer, &m_buffer, false)) {
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
        if (lua_pcall(L, 0, LUA_MULTRET, 0)) {
            lua_error(L);
        }

        lua_getglobal(L, "BIGGEngine");
        lua_getfield(L, -1, "Callbacks");
        lua_getfield(L, -1, "Init");
        if (!lua_isfunction(L, -1)) {
            BIGG_LOG_WARN("no Init function detected for script '{}'.", name);
            lua_pop(L, 3); // pop BIGGEngine, Callbacks, Init
            return;
        }

        if (lua_pcall(L, 0, 0, 0)) {
            lua_error(L); // pop Init.
        }
        // TODO repeat for all other callbacks

        lua_pop(L, 2); // pop BIGGEngine, Callbacks.
    }

    int ScriptHandle::writer(lua_State *L, const void *p, size_t sz, void *ud) {
        Buffer *buf = static_cast<Buffer *>(ud);
        buf->push(p, sz);
        return 0;
    }

    const char *ScriptHandle::reader(lua_State *, void *ud, size_t *size) {
        Buffer *buf = static_cast<Buffer *>(ud);
        *size = buf->size();
        return static_cast<const char *>(buf->data());
    }

    // ------------------- LUA Functions --------------------------------------------

    std::pair<int, entt::hashed_string> getScriptData(lua_State* L) {
        int subscribePriority = lua_tointeger(L, -1);
        lua_pop(L, 1);

        // get lua scriptName with lua_getglobal("sciptName")
        lua_getglobal(L, "BIGGEngine"); // gets _ENV.BIGGEnggine
        lua_getfield(L, -1, "ScriptName");  // pushes the value of ScriptName onto the stack
        if(!lua_isstring(L, -1)) {
            BIGG_LOG_CRITICAL("variable 'BIGGScriptName' is not a string. Failed to subscribe this script!");
            return {-1, entt::hashed_string{"Unknown script"}};
        }
        entt::hashed_string scriptName = entt::hashed_string{lua_tostring(L, -1)};
        lua_pop(L, 1);  // pop ScriptName.
        return {subscribePriority, scriptName};
    }

    void setupScriptState(lua_State* L, entt::hashed_string scriptName) {
        // Get handle for access to its cache buffer.
        Buffer* buff = getScriptBuffer(scriptName);

        // Load script
        if(lua_load(L, Scripting::ScriptHandle::reader, buff, scriptName.data(), "b")) {
            lua_error(L);
        }

        // Clear the existing state TODO maybe optional?
        lua_getglobal(L, "BIGGEngine"); // push _ENV.BIGGEngine to stack

        lua_pushstring(L, scriptName.data());     // push new name
        lua_setfield(L, -2, "ScriptName"); // pops name from stack, but keeps table

        lua_createtable(L, 0, 0);       // reset fields which may have been changed by another script.
        lua_setfield(L, -2, "Callbacks");  // to an empty table.
        lua_createtable(L, 0, 0);
        lua_setfield(L, -2, "Persistent");

        lua_pop(L, 1);                         // pop table BIGGEngine

        // By calling the script, we populate the global environment with BIGGEngine.Callbacks.*
        if(lua_pcall(L, 0, LUA_MULTRET, 0)) {
            lua_error(L);
        }
    }

    template<typename T>
    bool setupScriptComponentStateAndRun(lua_State* L, entt::hashed_string scriptName, std::string_view eventName, int numArgs, T lambda) {
        // for each ScriptComponent
        auto view = ECS::get().view<ScriptComponent>();
        for(auto [entity, scriptComponent] : view.each()) {

            if(scriptComponent.name != scriptName.value()) continue;

            // TODO load persistent data (something like below)
            // lua_pushinteger(entity);
            // lua_setglobal(BIGGEngine.this);

            // call lua function
            // get lua fuction
            if(lua_getglobal(L, "BIGGEngine") == LUA_TNIL) {
                BIGG_LOG_WARN("lua BIGGEngine is nil!");
                lua_pop(L, 1); // pop the nil from lua_getglobal()
                return false;
            }
            if(lua_getfield(L, -1, "Callbacks") == LUA_TNIL) {
                BIGG_LOG_WARN("lua BIGGEngine.Callbacks is nil!");
                lua_pop(L, 2); // pop everything
                return false;
            }
            if(lua_getfield(L, -1, eventName.data()) == LUA_TNIL) {
                BIGG_LOG_WARN("lua BIGGEngine.Callbacks.{} is nil!", eventName.data());
                lua_pop(L, 3); // pop everything
                return false;
            }
            if (lua_isnil(L, -1) || !lua_isfunction(L, -1)) {
                BIGG_LOG_WARN("lua BIGGEngine.Callbacks.{} is not a function!", eventName.data());
                lua_pop(L, 3); // pop everything
                return false;
            }
            // push args
            lambda();
            // call lua function
            if(lua_pcall(L, numArgs, 1, 0)) {
                lua_error(L);
                return false;
            }
            // get return value
            if (!lua_isboolean(L, -1)) {
                // error! should have returned a bool!
                BIGG_LOG_WARN("lua BIGGEngine.Callbacks.{} didn't return a bool!", eventName.data());
                lua_pop(L, 1);
                return false;
            }
            bool ret = lua_toboolean(L, -1);
            lua_pop(L, 1); // pop the return value from the stack
            if(ret) return true;    // only the first instance of this script will be ran.
            continue;   // go on to next instance of ScriptComponent
            // get return and return it
        }
        return false;
    }

    // undefined function template so only specializations are legal
    // "the function returns (in C) the number of results it is leaving on the stack"
    template<typename Event>
    int l_subscribe(lua_State* L);

    template<> int l_subscribe<MouseButtonEvent>(lua_State* L) {
        _BIGG_PROFILE_CATEGORY_FUNCTION("script");

        if(lua_gettop(L) != 1) return 0; // need 1 arg for subscribe point.

        auto [subscribePriority, scriptNameInter] = getScriptData(L);

        auto scriptName = scriptNameInter;  // so it can be captured by lambda

        auto callback = [=](MouseButtonEvent event) -> bool {
            _BIGG_PROFILE_CATEGORY_SCOPE("script", "callback");

            lua_State* L = getLuaState();
            setupScriptState(L, scriptName);

            return setupScriptComponentStateAndRun(L, scriptName, "MouseButton", 3, [=](){
                lua_pushnumber(L, static_cast<int>(event.m_button));
                lua_pushnumber(L, static_cast<int>(event.m_action));
                lua_pushnumber(L, static_cast<int>(event.m_mods));
            });
        };

        Events::subscribe<MouseButtonEvent>(subscribePriority, callback);
        return 0;   // number of args lua pushed to the stack
    }

    const luaL_Reg mylib_funcs[] {
            {"subscribeMouseButton", l_subscribe<MouseButtonEvent>},
            {NULL, NULL},
    };

    int open_mylib(lua_State* L) {
        // open lib into global table
        luaL_newlib(L, mylib_funcs);
        return 1;
    }
} // anonymous namespace

 // --------------------------- Public things --------------------------------

    void init() {
        if (L == nullptr) {
            L = luaL_newstate();
            luaL_openlibs(L);
            luaL_requiref(L, "mylib", open_mylib, 1);
        }
    }

    void shutdown() {
        BIGG_PROFILE_SHUTDOWN_FUNCTION;
        lua_close(L);
    }

    /// registers a script's name, loads, caches, and runs the script.
    void registerScript(entt::hashed_string name, std::string_view filepath) {
        // If a key equivalent to k already exists in the container, does nothing.
        // calls constructor of ScriptHandle
        m_registeredScripts.try_emplace(name.value(), name, filepath);
    }
} // namepace Scripting

    ScriptComponent::ScriptComponent(entt::hashed_string name) : name(name.value()) {}

} // namespace BIGGEngine {
