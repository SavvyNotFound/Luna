#pragma once
#include <lunapch.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Luna {

    class CameraOrtho
    {
        public:
            CameraOrtho(float left, float right, float bottom, float top);

            void SetPosition(const glm::vec3& position)
            {
                m_Position = position;
                RecalculateViewMatrix();
            }
            glm::vec3 GetPosition() const { return m_Position; }

            void SetRotation(const float& rotation)
            {
                m_Rotation = rotation;
                RecalculateViewMatrix();
            }
            const float GetRotation() const { return m_Rotation; }

            void SetProjection(float left, float right, float bottom, float top);
            const glm::mat4 GetProjectionMat() const { return m_ProjectionMatrix; }

            const glm::mat4 GetViewMat() const { return m_ViewMatrix; }
            const glm::mat4 GetViewProjectionMat() const { return m_ViewProjectionMatrix; }
        private:
            void RecalculateViewMatrix();
        private:
            glm::mat4 m_ProjectionMatrix = glm::mat4(0.0f);
            glm::mat4 m_ViewMatrix = glm::mat4(0.0f);
            glm::mat4 m_ViewProjectionMatrix = glm::mat4(0.0f);

            glm::vec3 m_Position = glm::vec3(0.0f);
            float m_Rotation = 0.0f;
    };

}
