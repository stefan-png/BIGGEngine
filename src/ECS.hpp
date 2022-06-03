#pragma once

#include <entt/entt.hpp>

namespace BIGGEngine {

    struct ECS {

    public:
        /// Gets the static entt::registry.
        static inline entt::registry& get() {
            return m_registry;
        }

    private:
        static inline entt::registry m_registry;
    };

} // namespace BIGGEngine