#pragma once
#include "Luna/Renderer/Camera.h"

namespace Luna {

    class SceneCamera : public Camera
    {
        public:
            enum class ProjectionType { Orthographic = 0, Perspective = 1 };
        public:
            SceneCamera();
            virtual ~SceneCamera() = default;

            ProjectionType GetProjectionType() const { return m_ProjectionType; }
            void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

            void SetViewportSize(uint32_t width, uint32_t height);

            // Projection
            void SetPerspective(float verticalFov, float nearClip, float farClip);

            void SetPerspectiveVerticalFOV(float verticalFOV) { m_PerspectiveFOV = glm::radians(verticalFOV); RecalculateProjection(); };
            float GetPerspectiveVerticalFOV() const { return glm::degrees(m_PerspectiveFOV); }

            void SetPerspectiveNear(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); };
            float GetPerspectiveNear() const { return m_PerspectiveNear; }

            void SetPerspectiveFar(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); };
            float GetPerspectiveFar() const { return m_PerspectiveFar; }


            // Orthographic
            void SetOrthographic(float size, float nearClip, float farClip);

            void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };
            float GetOrthographicSize() const { return m_OrthographicSize; }

            void SetOrthographicNear(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); };
            float GetOrthographicNear() const { return m_OrthographicNear; }

            void SetOrthographicFar(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); };
            float GetOrthographicFar() const { return m_OrthographicFar; }
        private:
            void RecalculateProjection();
        private:
            ProjectionType m_ProjectionType = ProjectionType::Orthographic;

            float m_PerspectiveFOV = glm::radians(45.0f);
            float m_PerspectiveNear = 0.01f, m_PerspectiveFar = +1000.0f;

            float m_OrthographicSize = 10.0f;
            float m_OrthographicNear = -1.0f, m_OrthographicFar = +1.0f;

            float m_AspectRatio = 1.0f;
    };

}
