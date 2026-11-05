#pragma once
#include <lunapch.h>

#include "Luna/Renderer/OrthoCamera.h"

#include "Luna/Core/Timestep.h"
#include "Luna/Events/ApplicationEvent.h"
#include "Luna/Events/MouseEvent.h"

namespace Luna {

    class CameraOrthoController
    {
        public:
            CameraOrthoController(float aspectRatio, bool enableRotation = false);

            void OnUpdate(Timestep ts);
            void OnEvent(Event& e);

            void Resize(float width, float height);

            CameraOrtho& GetCamera() { return m_Camera; }
            const CameraOrtho& GetCamera() const { return m_Camera; }
        private:
            bool OnMouseScrolled(MouseScrolledEvent& e);
            bool OnWindowResized(WindowResizeEvent& e);
        private:
            float m_AspectRatio;
            bool m_Rotation = false;
            float m_ZoomLevel = 1.0f;

            glm::vec3 m_Pos = glm::vec3(0.0f);
            float m_RotationZ = 0.0f;

            float m_CameraSpeed;
            float m_CameraRotationSpeed;

            CameraOrtho m_Camera;
    };

}
