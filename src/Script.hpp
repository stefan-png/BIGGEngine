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

namespace BIGGEngine {

    struct ScriptComponent {
        ScriptComponent(entt::hashed_string name);

        entt::hashed_string::hash_type name;
    };

namespace Scripting {

    void init();
    void shutdown();
    void registerScript(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority);

}   // namespace Scripting
}; // namespace BIGGEngine