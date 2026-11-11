#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

static int s_ColumnWidth = 140;

namespace Luna {

    class ExampleScript : public ScriptableEntity
    {
        public:
            void OnCreate()
            {
                LUNA_TRACE("Attached example script to {0}", GetComponent<TagComponent>().Tag);
            }
        private:
    };

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = s_ColumnWidth);
    static void DrawColor4Picker(const std::string& label, glm::vec4& color, float resetValue = 1.0f, float columnWidth = s_ColumnWidth);
    static bool DrawFloatControl(const std::string& label, float& value, float dragSpeed = 1.0f, float resetValue = 0.0f, float columnWidth = s_ColumnWidth);
    static void DrawCheckbox(const std::string& label, bool& value, float columnWidth = s_ColumnWidth);

    template<typename T, typename UIFunction>
    static void DrawComponent(const std::string& label, Entity entity, bool enableRemoving, UIFunction func)
    {
        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap;

        if (entity.HasComponent<T>())
        {
            auto& component = entity.GetComponent<T>();

            ImGui::Spacing();
            ImGui::Spacing();

            ImVec2 contentRegion = ImGui::GetContentRegionAvail();
            auto boldFont = ImGui::GetIO().Fonts->Fonts[0];

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            ImGui::Separator();
            ImGui::PushFont(boldFont);
            bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, label.c_str());
            ImGui::PopFont();
            ImGui::PopStyleVar();
            float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
            if (enableRemoving)
            {
                ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }
            }

            bool removeComponent = false;
            if (enableRemoving)
            {
                if (ImGui::BeginPopup("ComponentSettings"))
                {
                    if (ImGui::MenuItem("Remove component"))
                    {
                        removeComponent = true;
                    }
                    ImGui::EndPopup();
                }
            }
            if (opened)
            {
                // actual thing to render inside
                func(component);
                ImGui::TreePop();
            }

            if (removeComponent)
                entity.RemoveComponent<T>();

            ImGui::Spacing();
            ImGui::Spacing();
        }
    }

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
            if (entity)
                DrawEntityNode(entity);
        }
        ImGui::Unindent();

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {};

        // Right Click Blank Space
        if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
                m_Context->CreateEntity("Empty Entity");

            if (ImGui::MenuItem("Create Camera Entity"))
            {
                auto entity = m_Context->CreateEntity("Camera");
                entity.AddComponent<CameraComponent>();
            }

            ImGui::EndPopup();
        }

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

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        // Right Click Existing Entity
        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
                entityDeleted = true;

            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            if (m_SelectionContext == entity)
                m_SelectionContext = {};

            m_Context->DestroyEntity(entity);
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, tag.c_str());
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }

        {
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);

            if (ImGui::Button("Add Component"))
                ImGui::OpenPopup("AddComponent");

            if (ImGui::BeginPopup("AddComponent"))
            {
                if (!m_SelectionContext.HasComponent<CameraComponent>())
                    if (ImGui::MenuItem("Camera"))
                    {
                        m_SelectionContext.AddComponent<CameraComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
                    if (ImGui::MenuItem("Sprite Renderer"))
                    {
                        m_SelectionContext.AddComponent<SpriteRendererComponent>();
                        ImGui::CloseCurrentPopup();
                    }

                if (!m_SelectionContext.HasComponent<NativeScriptComponent>())
                    if (ImGui::MenuItem("Native Script"))
                    {
                        m_SelectionContext.AddComponent<NativeScriptComponent>().Bind<ExampleScript>();
                        ImGui::CloseCurrentPopup();
                    }

                ImGui::EndPopup();
            }
        }

        DrawComponent<TransformComponent>("Transform", entity, false, [](auto& component)
        {
                DrawVec3Control("Translation", component.Translation);
                glm::vec3 rt = glm::degrees(component.Rotation);
                DrawVec3Control("Rotation", rt);
                component.Rotation = glm::radians(rt);
                DrawVec3Control("Scale", component.Scale, 1.0f);
        });

        DrawComponent<CameraComponent>("Camera", entity, true, [](auto& component)
        {
            auto& camera = component.Camera;
            auto& primary = component.Primary;
            auto& fixedAspectRatio = component.FixedAspectRatio;

            const char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

            auto boldFont = ImGui::GetIO().Fonts->Fonts[0];
            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, s_ColumnWidth);
            ImGui::PushFont(boldFont);
            ImGui::Text("Type");
            ImGui::PopFont();
            ImGui::NextColumn();
            ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

            if (ImGui::BeginCombo("##proj-type", currentProjectionTypeString))
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
            ImGui::PopItemWidth();

            ImGui::Columns(1);
            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.GetOrthographicSize();
                if (DrawFloatControl("Size", orthoSize, 0.1f, 10.0f))
                    camera.SetOrthographicSize(orthoSize);

                float orthoNear = camera.GetOrthographicNear();
                if (DrawFloatControl("Near", orthoNear, 0.1f, -1.0f))
                    camera.SetOrthographicNear(orthoNear);

                float orthoFar = camera.GetOrthographicFar();
                if (DrawFloatControl("Far", orthoFar, 0.1f, 1.0f))
                    camera.SetOrthographicFar(orthoFar);
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float perspectiveVerticalFOV = camera.GetPerspectiveVerticalFOV();
                if (DrawFloatControl("FOV", perspectiveVerticalFOV, 0.5f, 45.0f))
                    camera.SetPerspectiveVerticalFOV(perspectiveVerticalFOV);

                float perspectiveNear = camera.GetPerspectiveNear();
                if (DrawFloatControl("Near", perspectiveNear, 0.1f, 0.001f))
                    camera.SetPerspectiveNear(perspectiveNear);

                float perspectiveFar = camera.GetPerspectiveFar();
                if (DrawFloatControl("Far", perspectiveFar, 0.1f, 10000.0f))
                    camera.SetPerspectiveFar(perspectiveFar);
            }

            DrawCheckbox("Primary", primary);
            DrawCheckbox("Fixed Aspect Ratio", fixedAspectRatio);
        });

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, true, [](auto& component)
        {
            DrawColor4Picker("Color: ", component.Color);
        });

        DrawComponent<NativeScriptComponent>("Native Script", entity, true, [](auto& component)
        {
            ImGui::Text("Nothing here, the script there, no impl tho :(");
        });
    }

    static void DrawCheckbox(const std::string& label, bool& value, float columnWidth)
    {
        bool returnVal = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());

        float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::NextColumn();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

        ImGui::Checkbox("##value", &value);

        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    static bool DrawFloatControl(const std::string& label, float& value, float dragSpeed, float resetValue, float columnWidth)
    {
        bool returnVal = false;
        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());

        float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;

        ImGui::NextColumn();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());

        if (ImGui::DragFloat("##Value", &value, dragSpeed, 0.0f, 1.0f, "%.3f"))
            returnVal = true;

        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();

        return returnVal;
    }

    static void DrawColor4Picker(const std::string& label, glm::vec4& color, float resetValue, float columnWidth)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 1, lineHeight + 1 };

        ImGui::Text("%s", label.c_str());

        ImGui::SameLine();

        ImVec4 btnCol = ImVec4{color.r, color.g, color.b, color.a};
        ImGui::PushStyleColor(ImGuiCol_Button, btnCol);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,btnCol);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, btnCol);
        if (ImGui::Button("##picker", buttonSize))
            ImGui::OpenPopup("##picker");

        if (ImGui::BeginPopup("##picker"))
        {
            ImGui::ColorPicker4("##color-picker", glm::value_ptr(color));
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::NextColumn();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("R", buttonSize))
            color.r = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##R", &color.r, 0.01f, 0.0f, 1.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("G", buttonSize))
            color.g = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##G", &color.g, 0.01f, 0.0f, 1.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("B", buttonSize))
            color.b = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##B", &color.b, 0.01f, 0.0f, 1.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.8f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.9f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.8f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("A", buttonSize))
            color.a = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##A", &color.a, 0.01f, 0.0f, 1.0f, "%.3f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

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
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();
    }
}
