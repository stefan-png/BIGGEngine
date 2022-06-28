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

namespace BIGGEngine {
namespace Scripting {

    void init();
    void shutdown();
    void registerScript(entt::hashed_string name, std::string_view filepath, uint16_t subscribePriority);

}   // namespace Scripting

    struct ScriptComponent {
        explicit ScriptComponent(entt::hashed_string::hash_type name);
        ~ScriptComponent();

        entt::hashed_string::hash_type m_name;
    };
}; // namespace BIGGEngine