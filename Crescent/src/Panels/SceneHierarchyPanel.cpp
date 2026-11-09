#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>

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

        ImGui::PushFont(nullptr, 22.0f);
        ImGui::Text("%s", sceneName.c_str());
        ImGui::PopFont();

        ImGui::Indent();
        for (auto entityID : m_Context->m_Registry.storage<entt::entity>())
        {
            Entity entity{ entityID, m_Context.get() };
            DrawEntityNode(entity);
        }
        ImGui::Unindent();

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
}
