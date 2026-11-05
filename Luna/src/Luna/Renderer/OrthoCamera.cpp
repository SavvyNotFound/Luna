#include <lunapch.h>
#include "OrthoCamera.h"

namespace Luna {

    CameraOrtho::CameraOrtho(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -16.0f, 16.0f)), m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void CameraOrtho::SetProjection(float left, float right, float bottom, float top)
    {
        LUNA_PROFILE_FUNCTION();

        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -16.0f, 16.0f);
        RecalculateViewMatrix();
    }

    void CameraOrtho::RecalculateViewMatrix()
    {
        LUNA_PROFILE_FUNCTION();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));   

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

}
