#pragma once

#include <SDL.h>

#include "imgui.h"

class GuiHandler {
 private:
  SDL_Window* window = nullptr;
  ImGuiIO* io = nullptr;
  SDL_GLContext gl_context;
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool done = false;

  void shutdown();
  void handle_events(SDL_Event*);

 public:
  bool init();
  void start_main_loop();
};
