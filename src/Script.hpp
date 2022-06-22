//
// Created by Stefan Antoszko on 2022-05-27.
//

#pragma once

#include "Core.hpp"
#include "Context.hpp" // for subscribe

#include <lua.hpp>
#include <unordered_map>

#include <string_view>  // for filename
#include "entt/core/hashed_string.hpp"  // for indexing into map of registered scripts

#include <string>   // for table_object
#include <unordered_map>

#include "rva/variant.hpp"

namespace BIGGEngine {
namespace Scripting {

    // since its not defined in lua
    using lua_Boolean = int;
    // recursive variant of a lua type
    using table_value = rva::variant<
            std::nullptr_t,                                // nil
            lua_Boolean,                                   // boolean
            lua_Integer,                                   // number - integer
            lua_Number,                                    // number - float
            std::string,                                   // string
            void*,                                         // light userdata
            std::unordered_map<std::string, rva::self_t>>; // table
            // no function
            // no full userdata
            // no thread

    using table_object = std::unordered_map<std::string, table_value>;

    void init();
    void shutdown();
    void registerScript(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority);

}   // namespace Scripting

    struct ScriptComponent {
        ScriptComponent(entt::hashed_string name);

        entt::hashed_string::hash_type m_name;
        Scripting::table_object m_globals;
    };
}; // namespace BIGGEngine