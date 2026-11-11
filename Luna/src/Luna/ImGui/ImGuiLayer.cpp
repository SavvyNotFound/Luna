#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

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
        io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto/static/Roboto-Bold.ttf", 18.0f);

        io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto/static/Roboto-Regular.ttf", 18.0f);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling + Theme
        ImGuiStyle& style = ImGui::GetStyle();

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

        SetupDarkTheming();
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

    void ImGuiLayer::SetupDarkTheming()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Base palette
        const ImVec4 bg_color       = ImVec4(0.098f, 0.102f, 0.110f, 1.00f); // #191A1C
        const ImVec4 bg_color_light = ImVec4(0.133f, 0.141f, 0.153f, 1.00f); // #222427
        const ImVec4 bg_surface     = ImVec4(0.169f, 0.180f, 0.196f, 1.00f); // #2B2E32
        const ImVec4 bg_active      = ImVec4(0.204f, 0.220f, 0.239f, 1.00f); // #34383D

        // Accent
        const ImVec4 accent         = ImVec4(0.424f, 0.616f, 0.792f, 1.00f); // #6C9DCA
        const ImVec4 accent_light   = ImVec4(0.514f, 0.694f, 0.863f, 1.00f); // #83B1DC
        const ImVec4 accent_dark    = ImVec4(0.322f, 0.498f, 0.659f, 1.00f); // #527FA8

        // Text
        const ImVec4 text           = ImVec4(0.835f, 0.847f, 0.863f, 1.00f); // #D5D8DC
        const ImVec4 text_disabled  = ImVec4(0.467f, 0.490f, 0.522f, 1.00f); // #777D85

        // Borders
        const ImVec4 border         = ImVec4(0.220f, 0.235f, 0.255f, 1.00f);

        // Window
        colors[ImGuiCol_WindowBg] = bg_color;
        colors[ImGuiCol_ChildBg] = bg_color;
        colors[ImGuiCol_PopupBg] = bg_color_light;

        // Text
        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = text_disabled;

        // Borders
        colors[ImGuiCol_Border] = border;
        colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

        // Frames
        colors[ImGuiCol_FrameBg] = bg_color_light;
        colors[ImGuiCol_FrameBgHovered] = bg_surface;
        colors[ImGuiCol_FrameBgActive] = bg_active;

        // Buttons
        colors[ImGuiCol_Button] = bg_surface;
        colors[ImGuiCol_ButtonHovered] = bg_active;
        colors[ImGuiCol_ButtonActive] = accent_dark;

        // Headers
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.28f, 0.36f, 0.65f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.36f, 0.47f, 0.75f);
        colors[ImGuiCol_HeaderActive] = accent_dark;

        // Separators
        colors[ImGuiCol_Separator] = border;
        colors[ImGuiCol_SeparatorHovered] = accent;
        colors[ImGuiCol_SeparatorActive] = accent_light;

        // Resize grips
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.424f, 0.616f, 0.792f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered] = accent;
        colors[ImGuiCol_ResizeGripActive] = accent_light;

        // Tabs
        colors[ImGuiCol_Tab] = bg_color;
        colors[ImGuiCol_TabHovered] = bg_surface;
        colors[ImGuiCol_TabActive] = bg_color_light;
        colors[ImGuiCol_TabSelectedOverline] = accent;

        // Title
        colors[ImGuiCol_TitleBg] = bg_color;
        colors[ImGuiCol_TitleBgActive] = bg_color_light;
        colors[ImGuiCol_TitleBgCollapsed] = bg_color;

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = bg_color;
        colors[ImGuiCol_ScrollbarGrab] = bg_surface;
        colors[ImGuiCol_ScrollbarGrabHovered] = bg_active;
        colors[ImGuiCol_ScrollbarGrabActive] = accent_dark;

        // Checkmarks and sliders
        colors[ImGuiCol_CheckMark] = accent_light;
        colors[ImGuiCol_SliderGrab] = accent;
        colors[ImGuiCol_SliderGrabActive] = accent_light;

        // Navigation
        colors[ImGuiCol_NavCursor] = accent_light;
        colors[ImGuiCol_NavHighlight] = accent;
        colors[ImGuiCol_NavWindowingHighlight] = accent_light;
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.45f);

        // Tables
        colors[ImGuiCol_TableHeaderBg] = bg_color_light;
        colors[ImGuiCol_TableBorderStrong] = border;
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.18f, 0.19f, 0.21f, 1.0f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0, 0, 0, 0);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1, 1, 1, 0.025f);

        // Drag and drop
        colors[ImGuiCol_DragDropTarget] = accent_light;

        // Style
        style.WindowRounding = 3.0f;
        style.ChildRounding = 3.0f;
        style.FrameRounding = 3.0f;
        style.PopupRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 3.0f;

        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;

        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 5.0f);
        style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
        style.IndentSpacing = 18.0f;
        style.ScrollbarSize = 12.0f;
        style.GrabMinSize = 10.0f;
    }
}
