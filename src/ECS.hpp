#pragma once

#include <entt/entt.hpp>

namespace BIGGEngine {

    struct ECS {

    public:
        /// Gets the static entt::registry.
        static inline entt::registry& get() {
            return m_registry;
        }
        static inline void shutdown() {
            m_registry.clear();
        }

    private:
        static inline entt::registry m_registry;
    };

} // namespace BIGGEngine