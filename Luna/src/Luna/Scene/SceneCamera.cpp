#include "SceneCamera.h"
#include <lunapch.h>

namespace Luna {

    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_OrthographicSize = size;   
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
        m_ProjectionType = ProjectionType::Orthographic;

        RecalculateProjection();
    }

    void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
    {
        m_PerspectiveFOV = glm::radians(verticalFOV);
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
        m_ProjectionType = ProjectionType::Perspective;

        RecalculateProjection();
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        height = height ? height : 1;
        m_AspectRatio = (float)width/(float)height;
        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Orthographic)
        {
            float orthoLeft =    -(m_OrthographicSize * m_AspectRatio * 0.5f);
            float orthoRight =    (m_OrthographicSize * m_AspectRatio * 0.5f);
            float orthoBottom =  -(m_OrthographicSize * 0.5f);
            float orthoTop =      (m_OrthographicSize * 0.5f);

            m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }
        else if (m_ProjectionType == ProjectionType::Perspective)
        {
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
        else
        {
            LUNA_CORE_ASSERT(false, "Invalid Camera Projection Type!");
        }
    }
}
