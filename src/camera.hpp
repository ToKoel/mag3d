#pragma once

#include <glm/glm.hpp>

#include "SDL_keycode.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

enum class Direction { LEFT, RIGHT, UP, DOWN };

class Camera {
 private:
  float field_of_view = 45.0f;

  glm::vec3 position = glm::vec3(0, 0, 5);
  glm::vec3 direction = glm::vec3(0, 0, 0);
  glm::vec3 up = glm::vec3(0, 1, 0);
  glm::vec3 right = glm::vec3(1, 0, 0);

  int x_mouse = 0;
  int y_mouse = 0;

  float speed = 0.005f;  // 3 units / second
  float mouse_speed = 0.0005f;

 public:
  void update_camera_directions(float, float, float);
  glm::mat4 get_view_matrix(float, float);
  void update_field_of_view(bool);
  void update_field_of_view(Sint32);
  void update_camera_position(SDL_KeyCode, float);
  void update_camera_position(const Uint8*, float);
  //
  // horizontal angle : toward -Z
  float horizontal_angle = 3.14f;
  // vertical angle : 0, look at the horizon
  float vertical_angle = 0.0f;
};