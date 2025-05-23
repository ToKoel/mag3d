#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 Camera::get_vp_matrix(const float window_width, const float window_height) const {
  const glm::mat4 projection = get_projection_matrix(window_width, window_height);
  const glm::mat4 view = get_view_matrix();

  return projection * view;
}

glm::mat4 Camera::get_view_matrix() const {
  return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::get_projection_matrix(const float window_width, const float window_height) const {
  return glm::perspective(
      glm::radians(field_of_view),
      window_width / window_height, 0.1f, 100.0f);
}

void Camera::update_camera_directions(const float deltaX, const float deltaY,
                                      const float delta_time) {
  horizontal_angle += deltaX * mouse_speed * delta_time;
  vertical_angle += deltaY * mouse_speed * delta_time;
  if (vertical_angle > 89.0f) vertical_angle = 89.0f;
  if (vertical_angle < -89.0f) vertical_angle = -89.0f;


  // Spherical to Cartesian conversion for camera position
  float theta = glm::radians(horizontal_angle);
  float phi = glm::radians(vertical_angle);

  auto target = glm::vec3(0.0f);
  auto radius = 5.0f;

  position = target + glm::vec3(
      radius * cos(phi) * sin(theta),
      radius * sin(phi),
      radius * cos(phi) * cos(theta)
  );

  // New direction vector from camera to target
  direction = glm::normalize(target - position);

  // Right and Up vectors for view matrix
  right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));
  up = glm::normalize(glm::cross(direction, right));

  // direction = glm::vec3(cos(vertical_angle) * sin(horizontal_angle),
  //                       sin(vertical_angle),
  //                       cos(vertical_angle) * cos(horizontal_angle));
  //
  // right = glm::vec3(sin(horizontal_angle - 3.14f / 2.0f), 0,
  //                   cos(horizontal_angle - 3.14f / 2.0f));
  //
  // up = glm::cross(right, direction);
}

void Camera::update_camera_position(const SDL_KeyCode direction, const float delta_time) {
  switch (direction) {
    case SDLK_LEFT:
      position -= right * delta_time * speed;
      break;
    case SDLK_RIGHT:
      position += right * delta_time * speed;
      break;
    case SDLK_UP:
      position += up * delta_time * speed;
      break;
    case SDLK_DOWN:
      position -= up * delta_time * speed;
      break;
    default:
      break;
  }
}

void Camera::update_camera_position(const Uint8* keyboardState,
                                    const float delta_time) {
  if (keyboardState[SDL_SCANCODE_RIGHT]) {
    position -= right * speed * delta_time;
  }
  if (keyboardState[SDL_SCANCODE_LEFT]) {
    position += right * speed * delta_time;
  }
  if (keyboardState[SDL_SCANCODE_UP]) {
    position += up * speed * delta_time;
  }
  if (keyboardState[SDL_SCANCODE_DOWN]) {
    position -= up * speed * delta_time;
  }
}

void Camera::update_field_of_view(const Sint32 wheel_pos) {
  if (wheel_pos > 0) {
    field_of_view -= 1.0f;
  }
  if (wheel_pos < 0) {
    field_of_view += 1.0f;
  }

  if (field_of_view < 1.0f) field_of_view = 1.0f;
  if (field_of_view > 90.0f) field_of_view = 90.0f;
}

void Camera::update_field_of_view(const bool in) {
  field_of_view = in ? field_of_view - 1.1f : field_of_view + 1.1f;
}