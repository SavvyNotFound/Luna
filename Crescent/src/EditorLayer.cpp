#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Luna {

    bool EditorLayer::s_SceneViewFullScr = false;
    bool EditorLayer::s_ViewportFocused = false;
    bool EditorLayer::s_ViewportHovered = false;

    EditorLayer::EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {
        LUNA_PROFILE_FUNCTION();

        m_LunaLogo = Texture2D::Create("Assets/Texture/LunaLogo.png");
        m_Texture = Texture2D::Create("Assets/Texture/RPGpack_sheet.png");
        m_CheckeredTexture = Texture2D::Create("Assets/Texture/Checkerboard.png");

        m_StairLTR = SubTexture2D::CreateFromCoords(m_Texture, {7, 6}, {64, 64});

        FrameBufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_ActiveScene = CreateRef<Scene>("Primary Scene");

        Entity square = m_ActiveScene->CreateEntity("Square");
        square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.2f, 1.0f});
        m_SquareEntity = square;

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity - 1");
        m_CameraEntity.AddComponent<CameraComponent>();

        m_CameraEntity2 = m_ActiveScene->CreateEntity("Camera Entity - 2");
        m_CameraEntity2.AddComponent<CameraComponent>().Primary = false;
        auto& cameraEntity2 = m_CameraEntity2.GetComponent<CameraComponent>();
        cameraEntity2.Camera.SetOrthographicSize(2.0f);

        class CameraController : public ScriptableEntity
        {
            public:
                void OnCreate()
                {
                    auto& tag = GetComponent<TagComponent>();
                    LUNA_TRACE("Attached camera controller script to {0}", tag.Tag);
                }

                void OnUpdate(Timestep ts)
                {
                    auto& transform = GetComponent<TransformComponent>().Transform;
                    auto& cc = GetComponent<CameraComponent>().Camera;
                    float speed = 5.0f;

                    if (s_ViewportFocused)
                    {
                        if (Input::IsKeyPressed(LunaKey_W))
                            transform[3][1] += speed * ts;
                        if (Input::IsKeyPressed(LunaKey_S))
                            transform[3][1] -= speed * ts;
                        if (Input::IsKeyPressed(LunaKey_D))
                            transform[3][0] += speed * ts;
                        if (Input::IsKeyPressed(LunaKey_A))
                            transform[3][0] -= speed * ts;
                    }
                }

                void OnEvent(Event &e)
                {
                    EventDispatcher dispatcher(e);
                    if (s_ViewportFocused && s_ViewportHovered)
                        dispatcher.Dispatch<MouseScrolledEvent>(LUNA_BIND_EVENT_FN(CameraController::Zoom));
                }

                void OnDestroy()
                {
                }
            private:
                bool Zoom(MouseScrolledEvent& e)
                {
                    auto& camera = GetComponent<CameraComponent>().Camera;
                    float orthoSize = camera.GetOrthographicSize() - e.GetYOffset();
                    orthoSize = orthoSize >= 1 ? orthoSize : 1;

                    camera.SetOrthographicSize(orthoSize);
                    return true;  
                }
        };

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach()
    {
        LUNA_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        LUNA_PROFILE_FUNCTION();

        // -- Update --
        m_FrameTime = ts;

        Renderer2D::ResetStats();
        // -- Render --
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.15f, 0.15f, 0.15f, 1.00f});
        RenderCommand::Clear();

        // Update Scene
        m_ActiveScene->OnUpdate(ts);

        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnImGuiRender()
    {
        LUNA_PROFILE_FUNCTION();

        static bool dockspaceOpen = true;
        static bool opt_fullscreen_presistent = true;
        bool opt_fullscreen = opt_fullscreen_presistent;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Crescent Editor", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();

        // -- Scene Viewport --
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Scene Viewport");

        s_ViewportFocused = ImGui::IsWindowFocused();
        s_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(s_ViewportFocused && s_ViewportHovered);

        ImVec2 sceneViewerPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize != glm::vec2{sceneViewerPanelSize.x, sceneViewerPanelSize.y} && sceneViewerPanelSize.x > 0 && sceneViewerPanelSize.y > 0)
        {
            m_Framebuffer->Resize((uint32_t)sceneViewerPanelSize.x, (uint32_t)sceneViewerPanelSize.y);
            m_ViewportSize = {sceneViewerPanelSize.x, sceneViewerPanelSize.y};

            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        uint32_t sceneID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(static_cast<int>(sceneID), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar();

        if (!s_SceneViewFullScr)
        {
            Renderer2D::Statistics stats = Renderer2D::GetStats();

            ImGui::Begin("Luna-Engine");
            ImGui::SeparatorText("Info");
            ImGui::Text("Frame time: %f s\n", m_FrameTime);
            ImGui::Text("FPS: %f", (1.0f / m_FrameTime));
            ImGui::Text("Draw call: %u", stats.DrawCalls);
            ImGui::Text("Quad count: %u", stats.QuadCount);
            ImGui::Text("Vertex count: %u", stats.GetTotalVertexCount());
            ImGui::Text("Index count: %u", stats.GetTotalIndexCount());
            ImGui::End();

            m_SceneHierarchyPanel.OnImGuiRender();

            ImGui::Begin("Random");
            if (m_SquareEntity)
            {
                ImGui::Separator();
                std::string tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
                ImGui::Text("%s", tag.c_str());

                auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
                ImGui::ColorEdit4("Quad Color", glm::value_ptr(squareColor));
                ImGui::Separator();
            }
            {
                if (ImGui::Checkbox("Primary Camera", &m_PrimaryCamera))
                {
                    m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
                    m_CameraEntity2.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
                }

                ImGui::Separator();

                {
                    ImGui::DragFloat2("Transform: ", glm::value_ptr(m_CameraEntity2.GetComponent<TransformComponent>().Transform[3]), 0.1f);
                    auto& camera = m_CameraEntity2.GetComponent<CameraComponent>().Camera;
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize, 0.1f))
                        camera.SetOrthographicSize(orthoSize);
                }
            }
            ImGui::End();
        }
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_ActiveScene->OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(LUNA_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        const auto& keycode = e.GetKeyCode();

        if (keycode == LunaKey_F11)
        {
            s_SceneViewFullScr = !s_SceneViewFullScr;
            return true;
        }

        return false;
    }

}
