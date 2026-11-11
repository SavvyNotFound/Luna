#include <Luna.h>
#include <LunaEntryPoint.h>

#include "EditorLayer.h"

namespace Luna {

    ApplicationSpecification g_Spec{ .Name = "Crescent", .Width = 1600, .Height = 900 };
    class Crescent : public Application
    {
        public:
            Crescent()
                : Application(g_Spec)
            {
                PushLayer(new EditorLayer());
            }

            ~Crescent()
            {
            }
        private:

    };

    Application* CreateApplication()
    {
        return new Crescent();
    }
}
