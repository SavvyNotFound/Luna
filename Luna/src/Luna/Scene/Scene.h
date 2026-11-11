#pragma once
#include <lunapch.h>

#include "Luna/Events/Event.h"
#include "Luna/Core/Timestep.h"
#include <entt/entt.hpp>

namespace Luna {

    class Entity;

    class Scene
    {
        public:
            Scene(const std::string& name = "Scene");
            ~Scene();

            Entity CreateEntity(const std::string& name = std::string());
            void DestroyEntity(Entity entity);

            void OnUpdate(Timestep ts);
            void OnEvent(Event& e);

            void OnViewportResize(uint32_t width, uint32_t height);

            std::string GetName() const { return m_Name; }
            void SetName(const std::string& name) { m_Name = name; }
        private:
            template<typename T>
            void OnComponentAdded(Entity entity, T& component);
        private:
            entt::registry m_Registry;
            std::string m_Name;
            uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

            friend class Entity;
            friend class SceneSerializer;
            friend class SceneHierarchyPanel;
    };

}
