#pragma once

#include <Luna.h>

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
        private:
            bool OnKeyPressed(KeyPressedEvent& e);
        private:
            Ref<Framebuffer> m_Framebuffer;

            bool SceneViewFullScr = false;
            bool m_ViewportFocused = false;
            bool m_ViewportHovered = false;
            glm::vec2 m_ViewportSize = {1.0f, 1.0f};

            Ref<Shader> m_Shader;
            CameraOrthoController m_CameraController;

            Ref<Texture2D> m_Texture;
            Ref<Texture2D> m_LunaLogo;
            Ref<Texture2D> m_CheckeredTexture;

            Ref<SubTexture2D> m_StairLTR;

            Ref<Scene> m_ActiveScene;

            Entity m_SquareEntity;
            float m_Rotation = 0.0f;

            float m_FrameTime = 0.0f;
    };

}
