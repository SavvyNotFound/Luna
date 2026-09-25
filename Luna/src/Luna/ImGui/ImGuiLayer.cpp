#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "JetBrainMonoFont.embeded"

#include "Luna/Core/Log.h"
#include "Luna/Core/Core.h"
#include "Luna/Core/Application.h"

// Temp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Luna {

    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer")
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        LUNA_PROFILE_FUNCTION();

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
        float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

        // From imgui_impl_opengl3 main file
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Font
        ImFontConfig fontConfig;
        strcpy(fontConfig.Name, "JetBrainsMono-Regular");
        ImFont* myFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
            JetBrainsMono_Regular,
            16.0f,
            &fontConfig
        );
        io.FontDefault = myFont;

        strcpy(fontConfig.Name, "JetBrainsMono-Bold");
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(
            JetBrainsMono_Bold,
            16.0f,
            &fontConfig
        );

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling + Theme
        ImGuiStyle& style = ImGui::GetStyle();

        ImVec4* colors = style.Colors;
        
        // Base colors
        const ImVec4 bg_color        = ImVec4(0.102f, 0.102f, 0.118f, 1.00f);
        const ImVec4 bg_color_light  = ImVec4(0.118f, 0.118f, 0.141f, 1.00f);
        const ImVec4 accent          = ImVec4(0.537f, 0.706f, 0.980f, 1.00f);
        const ImVec4 accent_light    = ImVec4(0.690f, 0.788f, 1.000f, 1.00f);
        const ImVec4 accent_dark     = ImVec4(0.400f, 0.580f, 0.850f, 1.00f);
        const ImVec4 text            = ImVec4(0.878f, 0.941f, 1.000f, 1.00f);
        const ImVec4 text_disabled   = ImVec4(0.467f, 0.467f, 0.467f, 1.00f);        

        // Window
        colors[ImGuiCol_WindowBg] = bg_color;
        colors[ImGuiCol_ChildBg] = bg_color;
        colors[ImGuiCol_PopupBg] = bg_color_light;
        
        // Text
        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = text_disabled;
        
        // Frames
        colors[ImGuiCol_FrameBg] = bg_color_light;
        colors[ImGuiCol_FrameBgHovered] = accent;
        colors[ImGuiCol_FrameBgActive] = accent_dark;
        
        // Buttons
        colors[ImGuiCol_Button] = accent;
        colors[ImGuiCol_ButtonHovered] = accent_light;
        colors[ImGuiCol_ButtonActive] = accent_dark;
        
        // Headers
        colors[ImGuiCol_Header] = accent;
        colors[ImGuiCol_HeaderHovered] = accent_light;
        colors[ImGuiCol_HeaderActive] = accent_dark;
        
        // Tabs
        colors[ImGuiCol_Tab] = bg_color_light;
        colors[ImGuiCol_TabHovered] = accent_light;
        colors[ImGuiCol_TabSelected] = accent;
        colors[ImGuiCol_TabSelectedOverline] = accent_light;
        
        // Title
        colors[ImGuiCol_TitleBg] = bg_color;
        colors[ImGuiCol_TitleBgActive] = bg_color_light;
        colors[ImGuiCol_TitleBgCollapsed] = bg_color;
        
        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = bg_color;
        colors[ImGuiCol_ScrollbarGrab] = bg_color_light;
        colors[ImGuiCol_ScrollbarGrabHovered] = accent;
        colors[ImGuiCol_ScrollbarGrabActive] = accent_dark;
        
        // Style properties
        style.WindowRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImGuiLayer::OnEvent(Event& event)
    {
        if (!m_BlockEvents)
        {
            ImGuiIO& io = ImGui::GetIO();
            event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }

    void ImGuiLayer::OnDetach()
    {
        LUNA_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnImGuiRender()
    {
    }

    void ImGuiLayer::Begin()
    {
        LUNA_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::End()
    {
        LUNA_PROFILE_FUNCTION();

        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
}
