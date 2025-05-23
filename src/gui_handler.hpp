#pragma once

#include <SDL.h>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "imgui.h"

class GuiHandler {
  SDL_Window *window = nullptr;
  ImGuiIO *io = nullptr;
  SDL_GLContext gl_context = nullptr;
  Camera camera{};

  glm::vec3 light_position = glm::vec3(4.0f, 4.0f, 4.0f);
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool done{false};
  uint64_t last_time{0};
  uint64_t now_time{0};
  double delta_time{0.0};
  bool dragging{false};
  uint32_t window_width{1028};
  uint32_t window_height{768};

  void shutdown() const;
  void handle_events(const SDL_Event *);
  void draw_control_window(glm::vec3*, glm::vec3*);

public:
  void init();
  void start_main_loop();
};
