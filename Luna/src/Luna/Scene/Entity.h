#pragma once
#include <lunapch.h>

#include <entt/entt.hpp>
#include <Luna/Scene/Scene.h>

namespace Luna {

    class Entity
    {
        public:
            Entity() = default;
            Entity(entt::entity entityHandle, Scene* scene);
            Entity(const Entity& other) = default;

            template<typename T, typename... Args>
            T& AddComponent(Args&&... args)
            {
                LUNA_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
                T& component = m_ParentScene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
                m_ParentScene->OnComponentAdded<T>(*this, component);
                return component;
            }

            template<typename T>
            [[nodiscard]] T& GetComponent()
            {
                LUNA_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
                return m_ParentScene->m_Registry.get<T>(m_EntityHandle);
            }

            template<typename T>
            bool HasComponent()
            {
                return m_ParentScene->m_Registry.all_of<T>(m_EntityHandle);
            }

            template<typename T>
            void RemoveComponent()
            {
                LUNA_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
                m_ParentScene->m_Registry.remove<T>(m_EntityHandle);
            }

            operator bool() const
            {
                if (m_ParentScene)
                    return m_ParentScene->m_Registry.valid(m_EntityHandle);
                else
                    return m_EntityHandle != entt::null;
            }
            operator entt::entity() const { return m_EntityHandle; }
            operator uint32_t() const { return (uint32_t)m_EntityHandle; }

            bool operator==(const Entity& other) const
            {
                return m_EntityHandle == other.m_EntityHandle && m_ParentScene == other.m_ParentScene;
            }

            bool operator!=(const Entity& other) const
            {
                return !(*this == other);
            }
        private:
            entt::entity m_EntityHandle{ entt::null };
            Scene* m_ParentScene = nullptr;
    };

}
