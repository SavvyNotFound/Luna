#pragma once

#include "Luna/Scene/Entity.h"

namespace Luna {

    class ScriptableEntity
    {
        public:
            ScriptableEntity() = default;
            virtual ~ScriptableEntity() = default;

            template<typename T>
            [[nodiscard]] T& GetComponent()
            {
                return m_Entity.GetComponent<T>();
            }
        private:
            Entity m_Entity;
            friend class Scene;
    };

}
