#pragma once

#include "Luna/Core/Layer.h"

#include "Luna/Events/Event.h"
#include "Luna/Events/KeyEvent.h"
#include "Luna/Events/MouseEvent.h"
#include "Luna/Events/ApplicationEvent.h"

namespace Luna {

    class ImGuiLayer : public Layer
    {
        public:
            ImGuiLayer();
            ~ImGuiLayer();

            virtual void OnAttach() override;
            virtual void OnDetach() override;
            virtual void OnImGuiRender() override;
            virtual void OnEvent(Event& event) override;

            void Begin();
            void End();

            void BlockEvents(bool block) { m_BlockEvents = block; }
        private:
            bool m_BlockEvents = false;
            float m_Time = 0.0f;
    };
}
