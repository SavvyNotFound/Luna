#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Luna {

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        auto sceneName = m_Context->GetName();

        ImGui::PushFont(nullptr, 24.0f);
        ImGui::Text("%s", sceneName.c_str());
        ImGui::PopFont();

        ImGui::Indent();
        for (auto entityID : m_Context->m_Registry.storage<entt::entity>())
        {
            Entity entity{ entityID, m_Context.get() };
            DrawEntityNode(entity);
        }
        ImGui::Unindent();

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {};

        ImGui::End();

        ImGui::Begin("Properties");
        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        if (opened)
        {
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

        const ImVec4 bg_color_light  = ImVec4(0.118f, 0.118f, 0.141f, 1.00f);
        ImGui::PushStyleColor(ImGuiCol_Header, bg_color_light);

        if (entity.HasComponent<TagComponent>())
        {
            ImGui::Spacing();
            ImGui::Spacing();
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, tag.c_str());
            ImGui::PushFont(nullptr, 22.0f);
            ImGui::TextUnformatted("Tag: ");
            ImGui::PopFont();
            ImGui::SameLine();
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
            ImGui::Spacing();
            ImGui::Spacing();
        }

        if (entity.HasComponent<TransformComponent>())
        {
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushFont(nullptr, 22.0f);
            bool opened = ImGui::TreeNodeEx("Tranform Component", flags);
            ImGui::PopFont();

            if (opened)
            {
                auto& transform = entity.GetComponent<TransformComponent>().Transform;
                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
                ImGui::TreePop();
            }
            ImGui::Spacing();
            ImGui::Spacing();
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushFont(nullptr, 22.0f);
            bool opened = ImGui::TreeNodeEx("Sprite Renderer Component", flags);
            ImGui::PopFont();

            if (opened)
            {
                auto& color = entity.GetComponent<SpriteRendererComponent>().Color;
                ImGui::ColorEdit4("Color", glm::value_ptr(color));

                ImGui::TreePop();
            }
            ImGui::Spacing();
            ImGui::Spacing();
        }

        if (entity.HasComponent<CameraComponent>())
        {
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushFont(nullptr, 22.0f);
            bool opened = ImGui::TreeNodeEx("Camera Component", flags);
            ImGui::PopFont();

            if (opened)
            {
                auto& cc = entity.GetComponent<CameraComponent>();
                auto& camera = cc.Camera;
                auto& primary = cc.Primary;
                auto& fixedAspectRatio = cc.FixedAspectRatio;

                ImGui::TextUnformatted("Ortho Size: ");
                ImGui::SameLine();
                float orthoSize = camera.GetOrthographicSize();
                if (ImGui::InputFloat("##Ortho Size", &orthoSize))
                    camera.SetOrthographicSize(orthoSize);

                ImGui::TextUnformatted("Ortho Near: ");
                ImGui::SameLine();
                float orthoNear = camera.GetOrthographicNear();
                if (ImGui::InputFloat("##Ortho Near", &orthoNear))
                    camera.SetOrthographicNear(orthoNear);

                ImGui::TextUnformatted("Ortho Far: ");
                ImGui::SameLine();
                float orthoFar = camera.GetOrthographicFar();
                if (ImGui::InputFloat("##Ortho Far", &orthoFar))
                    camera.SetOrthographicFar(orthoFar);

                ImGui::Checkbox("Primary", &primary);
                ImGui::Checkbox("Fixed Aspect Ratio", &fixedAspectRatio);

                ImGui::TreePop();
            }

            ImGui::Spacing();
            ImGui::Spacing();
        }

        if (entity.HasComponent<NativeScriptComponent>())
        {
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushFont(nullptr, 22.0f);
            bool opened = ImGui::TreeNodeEx("Native Script Component", flags);
            ImGui::PopFont();

            if (opened)
            {

                ImGui::TreePop();
            }

            ImGui::Spacing();
            ImGui::Spacing();
        }

        ImGui::PopStyleColor();
    }
}
