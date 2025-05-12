#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 Camera::get_view_matrix(float window_width, float window_height) {
  glm::mat4 projection = glm::perspective(
      glm::radians(field_of_view),
      static_cast<glm::float32_t>(window_width / window_height), 0.1f, 100.0f);

  // Camera matrix
  glm::mat4 view = glm::lookAt(position, position + direction, up);

  // Model matrix: an identity matrix (model will be at the origin)
  glm::mat4 model = glm::mat4(1.0f);
  // Our ModelViewProjection: multiplication of our 3 matrices
  glm::mat4 mvp =
      projection * view *
      model;  // Remember, matrix multiplication is the other way around

  return mvp;
}
void Camera::update_camera_directions(float deltaX, float deltaY,
                                      float delta_time) {
  horizontal_angle += deltaX * mouse_speed * delta_time;
  vertical_angle += deltaY * mouse_speed * delta_time;
  if (vertical_angle > 89.0f) vertical_angle = 89.0f;
  if (vertical_angle < -89.0f) vertical_angle = -89.0f;

  direction = glm::vec3(cos(vertical_angle) * sin(horizontal_angle),
                        sin(vertical_angle),
                        cos(vertical_angle) * cos(horizontal_angle));

  right = glm::vec3(sin(horizontal_angle - 3.14f / 2.0f), 0,
                    cos(horizontal_angle - 3.14f / 2.0f));

  up = glm::cross(right, direction);
}

void Camera::update_camera_position(SDL_KeyCode direction, float delta_time) {
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
                                    float delta_time) {
  if (keyboardState[SDL_SCANCODE_RIGHT]) {
    position += right * speed * delta_time;
  }
  if (keyboardState[SDL_SCANCODE_LEFT]) {
    position -= right * speed * delta_time;
  }
  if (keyboardState[SDL_SCANCODE_UP]) {
    position += up * speed * delta_time;
  }
  if (keyboardState[SDL_SCANCODE_DOWN]) {
    position -= up * speed * delta_time;
  }
}

void Camera::update_field_of_view(Sint32 wheel_pos) {
  if (wheel_pos > 0) {
    field_of_view -= 1.0f;
  }
  if (wheel_pos < 0) {
    field_of_view += 1.0f;
  }

  if (field_of_view < 1.0f) field_of_view = 1.0f;
  if (field_of_view > 90.0f) field_of_view = 90.0f;
}

void Camera::update_field_of_view(bool in) {
  if (in) {
    field_of_view -= 1.1f;
  } else {
    field_of_view += 1.1f;
  }
}