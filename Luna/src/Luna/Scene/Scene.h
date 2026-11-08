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

            Scene();
            ~Scene();

            Entity CreateEntity(const std::string& name = std::string());

            void OnUpdate(Timestep ts);
            void OnEvent(Event& e);

            void OnViewportResize(uint32_t width, uint32_t height);
        private:
            entt::registry m_Registry;
            uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

            friend class Entity;
    };

}
