#pragma once
#include <lunapch.h>

#include "Camera.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Luna {

    class Renderer
    {
        public:
            static void Init();
            static void Shutdown();
            static void OnWindowResize(int width, int height);

            static void BeginScene(Camera& camera, const glm::mat4& transform); // TODO: Take all params
            static void EndScene();

            static void Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const glm::mat4 transform = glm::mat4(1.0f));

            inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
        private:
            struct SceneData
            {
                glm::mat4 ViewProjectionMatrix;
            };

            static Scope<SceneData> m_SceneData;
    };
}
