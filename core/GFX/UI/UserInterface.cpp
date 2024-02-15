#include "UserInterface.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <magnet/Application.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

// This is a small wrapper over Dear ImGui to detach backend
// functionality from the WindowManager and Renderer

namespace Magnet::UI {

void setup(ArchiveManager& archiveMgr) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;

  ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
  ImGui_ImplOpenGL3_Init("#version 450");

  /* Load fonts */
  {
    ImFontConfig cfg;

    // Prevent ImGui and std::vector from both deallocating font
    // data, avoiding a 'double free or corruption' error on exit
    cfg.FontDataOwnedByAtlas = false;

    cfg.OversampleH = 1;
    cfg.OversampleV = 1;
    cfg.PixelSnapH = 1;

    static std::vector<unsigned char> roboto, firacode;

    archiveMgr.loadFile("fonts/Roboto-Regular.ttf", roboto);
    archiveMgr.loadFile("fonts/FiraCode-Regular.ttf", firacode);

    io.Fonts->ClearFonts();
    io.Fonts->AddFontFromMemoryTTF(roboto.data(), roboto.size(), 16.0f, &cfg);
    io.Fonts->AddFontFromMemoryTTF(firacode.data(), firacode.size(), 16.0f,
                                   &cfg);
  }
  /* DPI Scaling */
  {
    int width;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &width,
                           nullptr);

    float scale_factor = static_cast<float>(width) / 1920.0f;
    style.ScaleAllSizes(scale_factor);
  }
  /* Theming */
  {
    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.ChildRounding = 4.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 2.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(15.0f, 6.0f);
    style.FrameRounding = 2.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(4.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 13.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabMinSize = 7.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 1.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.28f, 0.28f, 0.28f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.145f, 0.145f, 0.145f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.145f, 0.145f, 0.145f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] =
      ImVec4(0.145f, 0.145f, 0.145f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] =
      ImVec4(0.2745f, 0.2745f, 0.2745f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.156f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.47f, 0.47f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.26f, 0.26f, 0.26f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.47f, 0.47f, 0.47f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.584f, 0.584f, 0.584f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.188f, 0.188f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] =
      ImVec4(0.227f, 0.227f, 0.247f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.156f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] =
      ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.586f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.586f);

    /* ImPlot style */
    ImPlotStyle* plot_style = &ImPlot::GetStyle();

    plot_style->MinorAlpha = 0.25f;
    plot_style->PlotPadding = ImVec2(0, 0);

    plot_style->Colors[ImPlotCol_Line] = IMPLOT_AUTO_COL;
    plot_style->Colors[ImPlotCol_Fill] = IMPLOT_AUTO_COL;
    plot_style->Colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    plot_style->Colors[ImPlotCol_MarkerFill] = IMPLOT_AUTO_COL;
    plot_style->Colors[ImPlotCol_ErrorBar] = IMPLOT_AUTO_COL;
    plot_style->Colors[ImPlotCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    plot_style->Colors[ImPlotCol_PlotBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    plot_style->Colors[ImPlotCol_PlotBorder] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    plot_style->Colors[ImPlotCol_LegendBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    plot_style->Colors[ImPlotCol_LegendBorder] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    plot_style->Colors[ImPlotCol_LegendText] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    plot_style->Colors[ImPlotCol_TitleText] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    plot_style->Colors[ImPlotCol_InlayText] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    plot_style->Colors[ImPlotCol_AxisText] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    plot_style->Colors[ImPlotCol_AxisGrid] = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    plot_style->Colors[ImPlotCol_AxisTick] = IMPLOT_AUTO_COL;       // TODO
    plot_style->Colors[ImPlotCol_AxisBg] = IMPLOT_AUTO_COL;         // TODO
    plot_style->Colors[ImPlotCol_AxisBgHovered] = IMPLOT_AUTO_COL;  // TODO
    plot_style->Colors[ImPlotCol_AxisBgActive] = IMPLOT_AUTO_COL;   // TODO
    plot_style->Colors[ImPlotCol_Selection] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    plot_style->Colors[ImPlotCol_Crosshairs] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);

    plot_style->Colormap = ImPlotColormap_Paired;
  }
}

void shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
}

void newFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void draw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace Magnet::UI