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
            glm::vec2 m_ViewportSize = {0.0f, 0.0f};

            Ref<Shader> m_Shader;
            CameraOrthoController m_CameraController;

            Ref<Texture2D> m_Texture;
            Ref<Texture2D> m_LunaLogo;
            Ref<Texture2D> m_CheckeredTexture;

            Ref<SubTexture2D> m_StairLTR;

            glm::vec4 m_QuadColor = { 0.2f, 0.3f, 0.8f, 1.0f };
            glm::vec2 m_QuadPos = { 0.0f, 0.0f };
            glm::vec2 m_QuadSize = { 1.0f, 1.0f };
            float m_Rotation = 0.0f;

            float m_FrameTime = 0.0f;
    };

}
