#pragma once
#include <lunapch.h>

#include <glm/glm.hpp>

#include "Luna/Events/Event.h"
#include "Luna/Scene/SceneCamera.h"
#include "Luna/Scene/ScriptableEntity.h"

namespace Luna {

    struct TagComponent 
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag)
        {}
    };

    struct TransformComponent 
    {
        glm::mat4 Transform{1.0f};

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& transform)
            : Transform(transform)
        {}

        operator glm::mat4() { return Transform; }
        operator const glm::mat4() const { return Transform; }
    };

    struct SpriteRendererComponent 
    {
        glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color)
            : Color(color)
        {}
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true; // TODO: Maybe move this option to scene
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct NativeScriptComponent
    {
        ScriptableEntity* Instance = nullptr;

        std::function<void()> InstantiateFunction;
        std::function<void()> DestroyInstantiateFunction;

        std::function<void(ScriptableEntity*)> OnCreateFunction;
        std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;
        std::function<void(ScriptableEntity*, Event&)> OnEventFunction;
        std::function<void(ScriptableEntity*)> OnDestroyFunction;

        template<typename T>
        void Bind()
        {
            InstantiateFunction = [&]() { Instance = new T(); };
            DestroyInstantiateFunction = [&]() { delete (T*)Instance; Instance = nullptr; };

            OnCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };
            OnUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { ((T*)instance)->OnUpdate(ts); };
            OnEventFunction = [](ScriptableEntity* instance, Event& e) { ((T*)instance)->OnEvent(e); };
            OnDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };
        }
    };

}
