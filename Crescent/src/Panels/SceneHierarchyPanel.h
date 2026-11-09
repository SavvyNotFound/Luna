#pragma once

#include "Luna/Core/Core.h"

#include "Luna/Scene/Scene.h"
#include "Luna/Scene/Entity.h"
#include "Luna/Scene/Component.h"

namespace Luna {

    class SceneHierarchyPanel
    {
        public:
            SceneHierarchyPanel() = default;
            SceneHierarchyPanel(const Ref<Scene>& context);

            void SetContext(const Ref<Scene>& context);

            void OnImGuiRender();
        private:
            void DrawEntityNode(Entity entity);
        private:
            Ref<Scene> m_Context;
            Entity m_SelectionContext;
    };

}
