/**
 * @file Topbar.cpp
 * @author Igor Alexey
 * @brief DevTools top bar widget
 */

#include "magnet/UI.hpp"

/* Forward Declarations */

static void Console(bool* enabled);
static void ExampleAppMainMenuBar(bool* enabled);

void Magnet::UI::Widgets::showDevTools() {
  static bool show_menu_bar = false;
  static bool show_console = false;

  Console(&show_console);
}

void Console(bool* enabled) {
  
}