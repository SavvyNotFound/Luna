#pragma once

#include "Luna/Scene/Entity.h"

namespace Luna {

    class ScriptableEntity
    {
        public:
            virtual ~ScriptableEntity() {}

            template<typename T>
            [[nodiscard]] T& GetComponent()
            {
                return m_Entity.GetComponent<T>();
            }

        protected:
            virtual void OnCreate() {};
            virtual void OnUpdate(Timestep ts) {};
            virtual void OnEvent(Event& e) {};
            virtual void OnDestroy() {};
        private:
            Entity m_Entity;
            friend class Scene;
    };

}
