#pragma once

#include <SDL.h>

#include <glm/glm.hpp>
#include <OpenGL/gl3.h>

#include "camera.hpp"
#include "imgui.h"
#include "solar_system_calculator.h"

class GuiHandler {
  SDL_Window *window = nullptr;
  ImGuiIO *io = nullptr;
  SDL_GLContext gl_context = nullptr;
  Camera camera;

  glm::vec3 light_position = glm::vec3(0.0);
  bool done{false};
  bool paused{false};
  uint64_t last_time{0};
  uint64_t now_time{0};
  double delta_time{0.0};
  float simulation_time_factor{10.0};
  double elapsed_simulation_time{0.0};
  bool dragging{false};
  uint32_t window_width{1028};
  uint32_t window_height{768};
  float inset_scale = 0.05f;

  void shutdown() const;
  static void start_imgui_frame();

public:
  void init();
  void start_main_loop();
};
