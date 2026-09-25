#include <lunapch.h>
#include "Application.h"

#include "Luna/Core/Log.h"
#include "Luna/Core/Input.h"
#include "Luna/Core/Timestep.h"
#include "Luna/Renderer/Renderer.h"

namespace Luna {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name)
        : m_Name(name)
    {
        LUNA_PROFILE_FUNCTION();
        LUNA_CORE_ASSERT(!s_Instance, "Application already exists");
        s_Instance = this;

        m_Window = Window::Create(WindowProps(name));
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer)
    {
        LUNA_PROFILE_FUNCTION();

        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        LUNA_PROFILE_FUNCTION();

        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::PopLayer(Layer* layer)
    {
        LUNA_PROFILE_FUNCTION();

        m_LayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void Application::PopOverlay(Layer* layer)
    {
        LUNA_PROFILE_FUNCTION();

        m_LayerStack.PopOverlay(layer);
        layer->OnDetach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::Run()
    {
        LUNA_PROFILE_FUNCTION();

        while(m_Running)
        {
            LUNA_PROFILE_SCOPE("RunLoop");

            float time = m_Window->GetTime();
            Timestep ts = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                {
                    LUNA_PROFILE_SCOPE("Layerstack layers update");
                    // Normal Layers Updates
                    for (Layer* layer : m_LayerStack)
                        layer->OnUpdate(ts);
                }

                {
                    LUNA_PROFILE_SCOPE("ImGui layer update");
                    // ImGui Rendering
                    m_ImGuiLayer->Begin();
                    for (Layer* layer : m_LayerStack)
                        layer->OnImGuiRender();
                    m_ImGuiLayer->End();
                }
            }

            m_Window->OnUpdate();
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    bool Application::OnWindowClose(WindowCloseEvent e)
    {
        m_Running = false;
        return true;
    };

    bool Application::OnWindowResize(WindowResizeEvent e)
    {
        LUNA_PROFILE_FUNCTION();

        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
        }
        else
        {
            m_Minimized = false;
            Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        }
        return false;
    };
}
