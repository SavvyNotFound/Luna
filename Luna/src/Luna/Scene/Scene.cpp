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
        m_Registry.group<NativeScriptComponent>().each([=, this](auto entity, auto& nsc)
        {
            // TODO: Move to Scene::OnPlay
            if (nsc.Instance)
            {
                nsc.Instance = nsc.InstantiateScript();
                nsc.DestroyScript(&nsc);
            }
        });
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = {m_Registry.create(), this};

        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;

        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        if (entity)
            m_Registry.destroy(entity);
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
        glm::mat4 cameraTransform;
        {
            auto group = m_Registry.group<>(entt::get<TransformComponent, CameraComponent>);
            for (auto entity : group)
            {
                auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary)
                {
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }


        if (mainCamera)
        {
            Renderer2D::BeginScene(*mainCamera, cameraTransform);

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group)
            {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
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
                nsc.Instance = nsc.InstantiateScript();
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

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component)
    {
        LUNA_CORE_ASSERT(false, "This shouldn't be here");
    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
    {
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
    {
    }

    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
    {
    }
}
