#pragma once

#include <SDL.h>

#include <glm/glm.hpp>

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
  int x_mouse = 0;
  int y_mouse = 0;
  float32_t field_of_view = 45.0f;

  glm::vec3 position = glm::vec3(0, 0, 5);
  glm::vec3 direction = glm::vec3(0, 0, 0);
  glm::vec3 up = glm::vec3(0, 1, 0);
  glm::vec3 right = glm::vec3(1, 0, 0);

  // horizontal angle : toward -Z
  float horizontal_angle = 3.14f;
  // vertical angle : 0, look at the horizon
  float vertical_angle = 0.0f;

  float speed = 0.005f;  // 3 units / second
  float mouse_speed = 0.0005f;

  uint32_t window_width = 1028;
  uint32_t window_height = 768;

  void shutdown();
  void handle_events(SDL_Event*);
  glm::mat4 get_view_matrix();

  uint64_t last_time = 0;
  uint64_t now_time = 0;
  double delta_time = 0.0;

  void update_camera_directions(float, float);

 public:
  bool init();
  void start_main_loop();
};
