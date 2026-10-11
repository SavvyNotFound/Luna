#pragma once
#include <lunapch.h>

#include <Luna/Core/Timestep.h>
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
        private:
            entt::registry m_Registry;

            friend class Entity;
    };

}
