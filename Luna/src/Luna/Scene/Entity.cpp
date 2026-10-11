#include <lunapch.h>
#include "Entity.h"

namespace Luna {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_ParentScene(scene)
    {
    }

}
