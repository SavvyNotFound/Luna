#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Luna {

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

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
                auto& tc = entity.GetComponent<TransformComponent>();

                // ImGui::DragFloat3("Translation", glm::value_ptr(tc.Translation), 0.1f);
                // ImGui::DragFloat3("Rotation", glm::value_ptr(tc.Rotation), 0.1f);
                // ImGui::DragFloat3("Scale", glm::value_ptr(tc.Scale), 0.1f);

                DrawVec3Control("Translation", tc.Translation);
                glm::vec3 rt = glm::degrees(tc.Rotation);
                DrawVec3Control("Rotation", rt);
                tc.Rotation = glm::radians(rt);
                DrawVec3Control("Scale", tc.Scale, 1.0f);

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

                const char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
                const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

                if (ImGui::BeginCombo("Camera Type", currentProjectionTypeString))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    ImGui::TextUnformatted("Size: ");
                    ImGui::SameLine();
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("##Size", &orthoSize, 0.1f))
                        camera.SetOrthographicSize(orthoSize);

                    ImGui::TextUnformatted("Near: ");
                    ImGui::SameLine();
                    float orthoNear = camera.GetOrthographicNear();
                    if (ImGui::DragFloat("##Near", &orthoNear, 0.1f))
                        camera.SetOrthographicNear(orthoNear);

                    ImGui::TextUnformatted("Far: ");
                    ImGui::SameLine();
                    float orthoFar = camera.GetOrthographicFar();
                    if (ImGui::DragFloat("##Far", &orthoFar, 0.1f))
                        camera.SetOrthographicFar(orthoFar);
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
                {
                    ImGui::TextUnformatted("FOV: ");
                    ImGui::SameLine();
                    float perspectiveVerticalFOV = camera.GetPerspectiveVerticalFOV();
                    if (ImGui::DragFloat("##FOV", &perspectiveVerticalFOV, 0.5f))
                        camera.SetPerspectiveVerticalFOV(perspectiveVerticalFOV);

                    ImGui::TextUnformatted("Near: ");
                    ImGui::SameLine();
                    float perspectiveNear = camera.GetPerspectiveNear();
                    if (ImGui::DragFloat("##Near", &perspectiveNear, 0.1f))
                        camera.SetPerspectiveNear(perspectiveNear);

                    ImGui::TextUnformatted("Far: ");
                    ImGui::SameLine();
                    float perspectiveFar = camera.GetPerspectiveFar();
                    if (ImGui::DragFloat("##Far", &perspectiveFar, 0.1f))
                        camera.SetPerspectiveFar(perspectiveFar);
                }

                ImGui::Checkbox("Primary", &primary);
                ImGui::Checkbox("Fixed Aspect Ratio", &fixedAspectRatio);

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
                auto& src = entity.GetComponent<SpriteRendererComponent>().Color;
                ImGui::ColorEdit4("Color", glm::value_ptr(src));

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

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());

        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }
}
