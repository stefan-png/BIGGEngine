//
// Created by Stefan Antoszko on 2022-05-27.
//

#pragma once

#include "Core.hpp"
#include <lua.hpp>
#include <unordered_map>
#include <string_view>

namespace BIGGEngine {

    struct Scripting {
    private:

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

        private:
            static int writer(lua_State* L, const void* p, size_t sz, void* ud);
            static const char *reader (lua_State *L, void *ud, size_t *size);

            friend struct Buffer;
        };
    public:

        /// registers a script's name, loads, caches, and runs the script.
        static void registerScript(std::string_view name, std::string_view filepath);
        /// returns a ScriptHandle with nullptr as name, filepath if input param name isn't registered.
        static Scripting::ScriptHandle getScript(std::string_view name);
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

    template<typename Event>
    static int l_subscribe(lua_State* L) {
        // we have a new stack with params at 1, 2, 3

        // get lua scriptName with lua_getglobal("sciptName")
        lua_getglobal(L, "BIGGScriptName");
        if(!lua_isstring(L, -1)) {
            BIGG_LOG_WARN("variable 'BIGGScriptName' is not a string. Cannot subscribe this script!");
            return LUA_ERRRUN;
        }
        std::string_view scriptName = lua_tostring(L, -1);
        lua_pop(L, 1);  // pop scriptName.

//        lua_load(L, Scripting::Buffer::reader)

        /*
         * to load the script:
         * lua_load(L, Buffer::reader, &m_buffer, name.data(), "b");
         * name::data is the debug name of the chunk
         * "b" indicated binary data.
         */

        auto cb = [scriptName](Event event){
            // get view of all entities with a ScriptComponent

            // for each script where ScriptComponent::name == scriptName

                // setglobal("this", entityID)
                // setglobal("persistent", ScriptComponent::persistent)
                // getglobal("update")

                // push(event.m_delta)
                // call(1, 1)

                // get output (boolean)

                // return output
        };
    }

}; // namespace BIGGEngine