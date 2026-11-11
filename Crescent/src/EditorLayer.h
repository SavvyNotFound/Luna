#pragma once

#include <Luna.h>
#include "Panels/SceneHierarchyPanel.h"

namespace Luna {

    class EditorLayer : public Layer
    {
        public:
            EditorLayer();
            virtual ~EditorLayer() = default;

            virtual void OnAttach() override;
            virtual void OnDetach() override;
            virtual void OnUpdate(Timestep ts) override;
            virtual void OnImGuiRender() override;
            virtual void OnEvent(Event& event) override;
        public:
            static bool s_SceneViewFullScr;
            static bool s_ViewportFocused;
            static bool s_ViewportHovered;
        private:
            void NewScene();
            void OpenScene();
            void SaveSceneAs();

            bool OnKeyPressed(KeyPressedEvent& e);
        private:
            Ref<Framebuffer> m_Framebuffer;

            glm::vec2 m_ViewportSize = {1.0f, 1.0f};

            Ref<Shader> m_Shader;

            Ref<Texture2D> m_Texture;
            Ref<Texture2D> m_LunaLogo;
            Ref<Texture2D> m_CheckeredTexture;

            Ref<SubTexture2D> m_StairLTR;

            Ref<Scene> m_ActiveScene;

            // Panels Begin
            SceneHierarchyPanel m_SceneHierarchyPanel;
            // Panels End

            Entity m_SquareEntity;
            float m_Rotation = 0.0f;

            bool m_PrimaryCamera = true;

            Entity m_CameraEntity;
            Entity m_CameraEntity2;

            float m_FrameTime = 0.0f;
    };

}
