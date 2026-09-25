#pragma once
#include <lunapch.h>

#include "Luna/Core/Window.h"
#include "Luna/Core/LayerStack.h"
#include "Luna/Events/Event.h"
#include "Luna/Events/ApplicationEvent.h"

#include "Luna/ImGui/ImGuiLayer.h"

namespace Luna {

    class Application
    {
        public:
            Application(const std::string& name = "Luna Application");
            virtual ~Application();

            void Run();
            void Close();

            void OnEvent(Event& e);

            void PushLayer(Layer* layer);
            void PushOverlay(Layer* layer);

            void PopLayer(Layer* layer);
            void PopOverlay(Layer* layer);

            ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
            const ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

            inline static Application& Get() { return *s_Instance; }
            inline Window& GetWindow() { return *m_Window; }
        private:
            bool OnWindowClose(WindowCloseEvent e);
            bool OnWindowResize(WindowResizeEvent e);
        private:
            Scope<Window> m_Window;
            ImGuiLayer* m_ImGuiLayer;
            bool m_Running = true;
            bool m_Minimized = false;
            LayerStack m_LayerStack;
            float m_LastFrameTime;
            std::string m_Name;
        private:
            static Application* s_Instance;
    };

    // To be defined by user
    Application* CreateApplication();
}
