#include "camera.hpp"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_impl_sdl2.h"

glm::mat4 Camera::get_vp_matrix() const {
  const glm::mat4 projection = get_projection_matrix();
  const glm::mat4 view = get_view_matrix();

  return projection * view;
}

glm::mat4 Camera::get_view_matrix() const {
  return glm::lookAt(position, position + direction, up);
}

void Camera::init(ImGuiIO* io, const float window_width, const float window_height, const float scale_factor) {
  this->window_width = window_width;
  this->window_height = window_height;
  this->high_dpi_scale_factor = scale_factor;
  this->io = io;
  update_camera_directions(1.0, 1.0, 0.0);
}

glm::mat4 Camera::get_projection_matrix() const {
  return glm::perspective(
      glm::radians(field_of_view),
      window_width / window_height, 0.1f, 100.0f);
}

void Camera::update_camera_directions(const float deltaX, const float deltaY,
                                      const float delta_time) {
  horizontal_angle += deltaX * mouse_speed * delta_time;
  vertical_angle += deltaY * mouse_speed * delta_time;

  if (vertical_angle > 179.0f) vertical_angle = 179.0f;
  if (vertical_angle < -179.0f) vertical_angle = -179.0f;

  const float theta = glm::radians(horizontal_angle);
  const float phi = glm::radians(vertical_angle);

  position = target + glm::vec3(
      radius * cos(phi) * sin(theta),
      radius * sin(phi),
      radius * cos(phi) * cos(theta)
  );

  direction = glm::normalize(target - position);

  constexpr auto world_up = glm::vec3(0.0f, 1.0f, 0.0f);
  if (vertical_angle < -90.0f || vertical_angle > 90.0f) {
    right = glm::normalize(glm::cross(direction, world_up));
  } else {
    right = glm::normalize(glm::cross(world_up, direction));
  }
  up = glm::normalize(glm::cross(direction, right));
}

bool Camera::handle_events(const float delta_time) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_MOUSEWHEEL) {
      update_field_of_view(event.wheel.y);
    }

    if (event.type == SDL_QUIT)
      return true;

    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT) {
      if (!io->WantCaptureMouse) {
        dragging = true;
      }
      SDL_SetRelativeMouseMode(SDL_TRUE);
        }
    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {
      dragging = false;
      SDL_SetRelativeMouseMode(SDL_FALSE);
      SDL_GetMouseState(&x_mouse,&y_mouse);
      current_mouse_ray = get_ray_from_mouse();
        }

    if (event.type == SDL_MOUSEMOTION && dragging) {
      update_camera_directions(static_cast<float>(event.motion.xrel),
                                      static_cast<float>(event.motion.yrel),
                                      delta_time);
    }
  }

  const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
  update_camera_position(keyboardState,delta_time);
  return false;
}

void Camera::update_camera_position(const Uint8* keyboardState,
                                    const float delta_time) {
  glm::vec3 offset;
  if (keyboardState[SDL_SCANCODE_RIGHT]) {
    offset = right * delta_time * speed;
    position -= offset;
    target -= offset;
  }
  if (keyboardState[SDL_SCANCODE_LEFT]) {
    offset = right * delta_time * speed;
    position += offset;
    target += offset;
  }
  if (keyboardState[SDL_SCANCODE_UP]) {
    offset = up * delta_time * speed;
    position += offset;
    target += offset;
  }
  if (keyboardState[SDL_SCANCODE_DOWN]) {
    offset = up * delta_time * speed;
    position -= offset;
    target -= offset;
  }
}

glm::vec3 Camera::get_direction() const {
  return direction;
}


glm::vec3 Camera::get_ray_from_mouse() const {
  const auto projection = get_projection_matrix();
  const auto view = get_view_matrix();

  const auto mouse_x = x_mouse * high_dpi_scale_factor;
  const auto mouse_y = y_mouse * high_dpi_scale_factor;

  const int ogl_mouse_y = window_height - mouse_y;

  const auto viewport = glm::vec4(0.0,0.0,window_width,window_height);

  const glm::vec3 screen_pos_near(mouse_x, ogl_mouse_y, 0.0f); // z = 0 -> near plane
  const glm::vec3 screen_pos_far (mouse_x, ogl_mouse_y, 1.0f); // z = 1 -> far plane

  const glm::vec3 ray_origin    = glm::unProject(screen_pos_near, view, projection, viewport);
  const glm::vec3 ray_target    = glm::unProject(screen_pos_far,  view, projection, viewport);

  return glm::normalize(ray_target - ray_origin);
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
