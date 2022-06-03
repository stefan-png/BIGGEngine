//
// Created by Stefan Antoszko on 2022-05-27.
//

#pragma once

#include "Core.hpp"
#include "ContextI.hpp" // for subscribe

#include <lua.hpp>
#include <unordered_map>
#include <string_view>

namespace BIGGEngine {

    struct Scripting {

        struct Buffer {
            Buffer();
            Buffer(void* data, size_t size);
            ~Buffer();

            void push(const void* data, size_t size);

            inline void* data() { return m_data; }
            inline size_t size() { return m_size; }

        private:
            void* m_data = nullptr;
            size_t m_size = 0;
            friend struct ScriptHandle;
        };

        struct ScriptHandle {
            ScriptHandle(std::string_view name, std::string_view filepath);

            // I'm expecting these to be hardcoded into the executable.
            std::string_view name;      // name to be registered
            std::string_view filepath;  // path to .lua script relative to executable

            Buffer m_buffer;            // buffer for caching the script.

            static int writer(lua_State* L, const void* p, size_t sz, void * ud);
            static const char *reader (lua_State *L, void * ud, size_t *size);

            friend struct Buffer;
        };

        /// registers a script's name, loads, caches, and runs the script.
        static void registerScript(std::string_view name, std::string_view filepath);
        /// returns a ScriptHandle with nullptr as name, filepath if input param name isn't registered.
        static Buffer* getScriptBuffer(std::string_view name);
        static lua_State* getL();
        static void shutdown();

    private:

        static inline lua_State* L = nullptr;
        static inline std::unordered_map<std::string_view, ScriptHandle> m_registeredScripts;
    };

    struct ScriptComponent{
        ScriptComponent(std::string_view name);

        std::string_view m_name;  // key into Scripting::m_registeredScripts;
        // TODO persistent data
    };

    static int l_subscribe(lua_State* L) {
        _BIGG_PROFILE_CATEGORY_FUNCTION("script");

        if(lua_gettop(L) != 1) return 0; // need 1 arg for subscribe point.

        int subscribePriority = lua_tointeger(L, -1);
        lua_pop(L, 1);

        // get lua scriptName with lua_getglobal("sciptName")
        lua_getglobal(L, "BIGGEngine"); // gets _ENV.BIGGEnggine
        lua_getfield(L, -1, "ScriptName");  // pushes the value of ScriptName onto the stack
        if(!lua_isstring(L, -1)) {
            BIGG_LOG_WARN("variable 'BIGGScriptName' is not a string. Failed to subscribe this script!");
            return LUA_ERRRUN;
        }
        std::string scriptName = lua_tostring(L, -1);
        lua_pop(L, 1);  // pop ScriptName.

        // START LAMBDA
        auto callback = [scriptName](Event* event) {
            _BIGG_PROFILE_CATEGORY_SCOPE("script", "callback");

            // Get handle for access to its cache buffer.

            Scripting::Buffer* buff = Scripting::getScriptBuffer(scriptName);

            lua_State* L = Scripting::getL();

            // Load callbacks
            if(lua_load(L, Scripting::ScriptHandle::reader, buff, scriptName.data(), "b")) {
                lua_error(L);
            }

            // Clear the existing state
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

            // for each ScriptComponent
            auto view = ECS::get().view<ScriptComponent>();
            for(auto [entity, scriptComponent] : view.each()) {

                if(scriptComponent.m_name != scriptName) continue;

                // TODO load persistent data (something like below)
                // lua_pushinteger(entity);
                // lua_setglobal(BIGGEngine.this);

                // call lua function
                if(event->m_type == Event::EventType::MouseButton) {
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
                    if(lua_getfield(L, -1, "MouseButton") == LUA_TNIL) {
                        BIGG_LOG_WARN("lua BIGGEngine.Callbacks.MouseButton is nil!");
                        lua_pop(L, 3); // pop everything
                        return false;
                    }
                    if (lua_isnil(L, -1) || !lua_isfunction(L, -1)) {
                        BIGG_LOG_WARN("lua BIGGEngine.Callbacks.MouseButton is not a function!");
                        lua_pop(L, 3); // pop everything
                        return false;
                    }
                    // push args
                    lua_pushnumber(L, static_cast<int>(static_cast<MouseButtonEvent *>(event)->m_button));
                    lua_pushnumber(L, static_cast<int>(static_cast<MouseButtonEvent *>(event)->m_action));
                    lua_pushnumber(L, static_cast<int>(static_cast<MouseButtonEvent *>(event)->m_mods));
                    // call lua function
                    if(lua_pcall(L, 3, 1, 0)) {
                        lua_error(L);
                        return false;
                    }
                    // get return value
                    if (!lua_isboolean(L, -1)) {
                        // error! should have returned a bool!
                        BIGG_LOG_WARN("lua BIGGEngine.Callbacks.MouseButton didn't return a bool!");
                        lua_pop(L, 1);
                        return false;
                    }
                    bool ret = lua_toboolean(L, -1);
                    lua_pop(L, 1); // pop the return value from the stack
                    return ret;
                }
                // get return and return it
                return false;

            }
            return false;
        }; // END LAMBDA

        ContextI::getInstance()->subscribe(subscribePriority, callback);
        return 0;
    }

    static const luaL_Reg mylib_funcs[] {
            {"subscribe", l_subscribe},
            {NULL, NULL},
    };

    int open_mylib(lua_State* L) {
        // open lib into global table
        luaL_newlib(L, mylib_funcs);
        return 1;
    }

}; // namespace BIGGEngine