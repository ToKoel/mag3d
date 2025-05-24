#include "gui_handler.hpp"
#include <OpenGL/gl3.h>
#include <imgui_impl_opengl3_loader.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <span>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include <SDL_opengl.h>

#include "solar_system_calculator.h"
#include "solar_system_graphics.h"

void GuiHandler::init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
  }

  // GL 3.2 Core + GLSL 150
  const auto glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  constexpr auto window_flags = static_cast<SDL_WindowFlags>(
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  window =
      SDL_CreateWindow("Mag3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       static_cast<std::int32_t>(window_width),
                       static_cast<std::int32_t>(window_height), window_flags);
  if (window == nullptr) {
    throw std::runtime_error("SDL_CreateWindow failed: " +
                             std::string(SDL_GetError()));
  }

  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    throw std::runtime_error("SDL_GL_CreateContext failed: " +
                             std::string(SDL_GetError()));
  }

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = &ImGui::GetIO();
  (void)io;
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  camera.init(io, static_cast<float>(window_width), static_cast<float>(window_height));
}

void GuiHandler::shutdown() const {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

glm::mat4 get_rotation_matrix(glm::vec3 direction) {
  glm::vec3 default_dir = glm::vec3(0.0f, 0.0f, 1.0f); // your model's default orientation
  glm::vec3 target_dir = glm::normalize(direction); // direction you want to rotate to

  // Compute rotation axis and angle
  glm::vec3 axis = glm::cross(default_dir, target_dir);
  float angle = acos(glm::clamp(glm::dot(default_dir, target_dir), -1.0f, 1.0f));

  // Handle the case when vectors are aligned or opposite
  glm::mat4 rotation;
  if (glm::length(axis) < 1e-6f) {
    if (glm::dot(default_dir, target_dir) > 0.0f) {
      // No rotation needed
      rotation = glm::mat4(1.0f);
    } else {
      // Rotate 180° around any perpendicular axis (e.g. X)
      rotation = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    }
  } else {
    axis = glm::normalize(axis);
    rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
  }
return rotation;
}

void GuiHandler::start_imgui_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}


void GuiHandler::start_main_loop() {

  SolarSystemCalculator solar_system_calculator;
  solar_system_calculator.init();

  SolarSystemGraphics solar_system_graphics(solar_system_calculator, camera);
  solar_system_graphics.init();

  double delta_time_seconds = 0.0;
  now_time = SDL_GetPerformanceCounter();
  last_time = now_time;

  while (!done) {
    now_time = SDL_GetPerformanceCounter();
    delta_time = static_cast<double>((now_time - last_time) * 1000) /
                 static_cast<double>(SDL_GetPerformanceFrequency());
    delta_time_seconds = static_cast<double>(now_time - last_time) /
                  static_cast<double>(SDL_GetPerformanceFrequency());
    last_time = now_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    done = camera.handle_events(static_cast<float>(delta_time));

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    start_imgui_frame();

    solar_system_graphics.draw_control_window();
    solar_system_graphics.draw_orbit_view();
    solar_system_graphics.draw_solar_system();

    if (!solar_system_calculator.paused) {
      const auto dt_days =static_cast<float>(delta_time_seconds) / 86400 * solar_system_calculator.simulation_time_factor;
      solar_system_calculator.elapsed_simulation_time += dt_days;
      solar_system_calculator.update_bodies_verlet(dt_days);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  shutdown();
}
