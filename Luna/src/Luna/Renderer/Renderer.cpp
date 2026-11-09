#include <lunapch.h>
#include "Renderer.h"

#include "Luna/Renderer/Renderer2D.h"

namespace Luna {

    Scope<Renderer::SceneData> Renderer::m_SceneData = CreateScope<Renderer::SceneData>();

    void Renderer::Init()
    {
        LUNA_PROFILE_FUNCTION();

        RenderCommand::Init(); 
        Renderer2D::Init();
    }

    void Renderer::Shutdown()
    {
        Renderer2D::Shutdown();
    }

    void Renderer::OnWindowResize(int width, int height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(Camera& camera, const glm::mat4& transform)
    {
        m_SceneData->ViewProjectionMatrix = camera.GetProjection() * glm::inverse(transform);
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const Ref<Shader> shader, const Ref<VertexArray>& vertexArray, const glm::mat4 transform)
    {
        shader->Bind();
        shader->SetMat4(m_SceneData->ViewProjectionMatrix, "u_ViewProjection");
        shader->SetMat4(transform, "u_ModelPosition");
        RenderCommand::DrawIndexed(vertexArray);
    }

}
