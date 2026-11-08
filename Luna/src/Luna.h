#pragma once

// -- Core --
#include <Luna/Core/Core.h>
#include <Luna/Core/Timestep.h>
// -- Core --

// -- Application --
#include <Luna/Core/Application.h>
// -- Application --

// -- Application Layers --
#include <Luna/Core/Layer.h>
#include <Luna/ImGui/ImGuiLayer.h>
// -- Application Layers --

// -- Inputs --
#include <Luna/Core/Input.h>
#include <Luna/Core/KeyCodes.h>
#include <Luna/Core/MouseButtonCodes.h>
// -- Inputs --

// -- Rendering --
// -- API Stuff --
#include <Luna/Renderer/Renderer.h>
#include <Luna/Renderer/Renderer2D.h>
#include <Luna/Renderer/RenderCommand.h>
// -- API Stuff --

// -- Resources --
#include <Luna/Renderer/Buffer.h>
#include <Luna/Renderer/Shader.h>
#include <Luna/Renderer/Texture.h>
#include <Luna/Renderer/SubTexture.h>
#include <Luna/Renderer/Framebuffer.h>
#include <Luna/Renderer/VertexArray.h>
#include <Luna/Renderer/OrthoCamera.h>
#include <Luna/Renderer/OrthoCameraController.h>
// -- Resources --
// -- Rendering --

// -- Scene --
#include <Luna/Scene/Scene.h>
#include <Luna/Scene/Entity.h>
#include <Luna/Scene/Component.h>
#include "Luna/Scene/SceneCamera.h"
#include "Luna/Scene/ScriptableEntity.h"
// --Scene --

// -- Logging --
// Bugs out without ostr sometimes
#include <spdlog/fmt/ostr.h>
#include <Luna/Core/Log.h>
// -- Logging --
