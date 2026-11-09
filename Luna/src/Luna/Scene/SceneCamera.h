#pragma once
#include "Luna/Renderer/Camera.h"

namespace Luna {

    class SceneCamera : public Camera
    {
        public:
            SceneCamera();
            virtual ~SceneCamera() = default;

            void SetOrthographic(float size, float nearClip, float farClip);
            void SetViewportSize(uint32_t width, uint32_t height);

            void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };
            float GetOrthographicSize() const { return m_OrthographicSize; }

            void SetOrthographicNear(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); };
            float GetOrthographicNear() const { return m_OrthographicNear; }

            void SetOrthographicFar(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); };
            float GetOrthographicFar() const { return m_OrthographicFar; }
        private:
            void RecalculateProjection();
        private:
            float m_OrthographicSize = 10.0f;
            float m_OrthographicNear = -1.0f, m_OrthographicFar = +1.0f;

            float m_AspectRatio = 1.0f;
    };

}
