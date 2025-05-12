#pragma once

#include <SDL.h>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "imgui.h"

class GuiHandler {
 private:
  SDL_Window* window = nullptr;
  ImGuiIO* io = nullptr;
  SDL_GLContext gl_context;
  Camera camera;
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool done = false;

  uint32_t window_width = 1028;
  uint32_t window_height = 768;

  void shutdown();
  void handle_events(SDL_Event*);
  glm::mat4 get_view_matrix();

  uint64_t last_time = 0;
  uint64_t now_time = 0;
  double delta_time = 0.0;

  bool dragging;

 public:
  GuiHandler();
  bool init();
  void start_main_loop();
};
