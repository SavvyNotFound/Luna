#include <lunapch.h>
#include "Scene.h"

#include <Luna/Renderer/Renderer.h>
#include <Luna/Renderer/Renderer2D.h>
#include <Luna/Scene/Component.h>

#include <Luna/Scene/Entity.h>

namespace Luna {

    Scene::Scene(const std::string& name)
        : m_Name(name)
    {
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};

        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    void Scene::OnUpdate(Timestep ts)
    {
        // Update Script
        {
            m_Registry.group<NativeScriptComponent>().each([=, this](auto entity, auto& nsc)
            {
                // TODO: Move to Scene::OnPlay
                if (!nsc.Instance)
                {
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity{ entity, this };
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->OnUpdate(ts);
            });
        }

        // Render 2D Sprites
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;
        {
            auto group = m_Registry.group<>(entt::get<TransformComponent, CameraComponent>);
            for (auto entity : group)
            {
                auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary)
                {
                    mainCamera = &camera.Camera;
                    cameraTransform = &transform.Transform;
                    break;
                }
            }
        }


        if (mainCamera)
        {
            Renderer2D::BeginScene(*mainCamera, *cameraTransform);

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::DrawQuad(transform, sprite.Color);
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::OnEvent(Event& e)
    {
        m_Registry.group<NativeScriptComponent>().each([&e, this](auto entity, auto& nsc)
        {
            if (!nsc.Instance)
            {
                nsc.InstantiateScript();
                nsc.Instance->m_Entity = Entity{ entity, this };
                nsc.Instance->OnCreate();
            }

            nsc.Instance->OnEvent(e);
        });
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        // Resize non fixed aspect ratio cams

        auto group = m_Registry.group<CameraComponent>();
        for (auto entity : group)
        {
            auto& cameraComponent = group.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }
}

// Some Usage of entt

// entt::entity entity1 = m_Registry.create();
//
// m_Registry.emplace<TransformComponent>(entity1, glm::mat4(1.0f));
//
// // must have every component defined components asked
// // option: reg.any_of
// // must have atleast one of the defined components asked
// if (m_Registry.all_of<TransformComponent>(entity1))
// {
//     TransformComponent& tranform = m_Registry.get<TransformComponent>(entity1);
//     tranform = glm::mat4(2.0f);
// }
//
// m_Registry.remove<TransformComponent>(entity1);
//
// // Get every entity with transform component
// auto view = m_Registry.view<TransformComponent>();
