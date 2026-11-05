#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Luna {

    class Camera
    {
        public:
            Camera(const glm::mat4& projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f))
                : m_Projection(projection)
            {
            }

            const glm::mat4& GetProjection() const { return m_Projection; }

            // TODO: Setting Camera's view, prospective / ortho
        private:
            glm::mat4 m_Projection;
    };

}
