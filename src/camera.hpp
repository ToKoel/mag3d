#pragma once

#include <glm/glm.hpp>

#include "imgui.h"
#include "SDL_events.h"
#include "SDL_keycode.h"

enum class Direction { LEFT, RIGHT, UP, DOWN };

class Camera {
  float field_of_view = 45.0f;

  glm::vec3 position = glm::vec3(0, 0, 5);
  glm::vec3 target = glm::vec3(0, 0, 0);
  glm::vec3 direction = glm::vec3(0, 0, 0);
  glm::vec3 up = glm::vec3(0, 1, 0);
  glm::vec3 right = glm::vec3(1, 0, 0);

  int x_mouse = 0;
  int y_mouse = 0;

  float speed = 0.005f; // 3 units / second
  float mouse_speed = 0.01f;
  bool dragging = false;
  float radius = 5.0f;

  ImGuiIO* io = nullptr;

public:
  void update_camera_directions(float, float, float);
  [[nodiscard]] glm::mat4 get_vp_matrix(float, float) const;
  [[nodiscard]] glm::mat4 get_view_matrix() const;
  [[nodiscard]] glm::mat4 get_projection_matrix(float, float) const;
  void update_field_of_view(bool);
  void update_field_of_view(Sint32);
  void update_camera_position(const Uint8 *, float);
  void init(ImGuiIO* io);
  bool handle_events(float);

  float horizontal_angle = 3.14f;
  float vertical_angle = 0.0f;
};