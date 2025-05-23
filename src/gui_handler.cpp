#include "gui_handler.hpp"
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <imgui_impl_opengl3_loader.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <span>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "load_shader.hpp"
#include <SDL_opengl.h>

#include "FileLoader.h"
#include "solar_system.h"

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

  camera.init(io);
}

bool SliderDouble(const char* label, double& value, const float min, const float max, const float factor) {
  auto temp = static_cast<float>(value) / factor;
  const bool changed = ImGui::SliderFloat(label, &temp, min, max,"%.1f", ImGuiSliderFlags_Logarithmic);
  if (changed) value = static_cast<double>(temp) * factor;
  return changed;
}

void GuiHandler::draw_control_window(SolarSystem& solar_system) {
  ImGui::Begin("Control");
  SliderDouble("Sun mass", solar_system.bodies[0].mass, 1.0f, 1000.0f, 1.0e29f);
  SliderDouble("Earth mass", solar_system.bodies[1].mass, 1.0f, 10000.0f, 1.0e24f);
  ImGui::SliderFloat("Simulation time factor", &simulation_time_factor, 1.0, 1000000.0, "%.0f", ImGuiSliderFlags_Logarithmic);
  ImGui::Checkbox("Pause", &paused);
  ImGui::Text("Time: %.1f days", elapsed_simulation_time);
  ImGui::End();
}

void GuiHandler::shutdown() const {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void draw_shape(const Shape shape, const GLuint program_id, glm::vec3 color) {
  constexpr auto number_of_vertices_per_triangle = 3;

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, shape.vertex_buffer_id);
  glVertexAttribPointer(0, // attribute 0. No particular reason for 0, but
                           // must match the layout in the shader.
                        3, // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        nullptr);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, shape.normal_buffer_id);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glUniform3f(glGetUniformLocation(program_id, "objectColor"), color.r, color.g,
              color.b);

  glDrawArrays(GL_TRIANGLES, 0,
               shape.number_of_triangles * number_of_vertices_per_triangle);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
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

void GuiHandler::set_lighting(const GLuint program_id) {
  // glUniform3f(glGetUniformLocation(program_id, "objectColor"), 1.0f, 1.0f,
  //             1.0f);
  glUniform3f(glGetUniformLocation(program_id, "lightColor"), 1.0f, 1.0f,
              1.0f);
  glUniform3fv(glGetUniformLocation(program_id, "lightPos"), 1,
               glm::value_ptr(light_position));
}

void GuiHandler::start_imgui_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void draw_2d_overlay(const Body& body, float& inset_scale) {
  ImGui::SetNextWindowSize(ImVec2(200, 200));
  ImGui::SetNextWindowPos(ImVec2(10, 10)); // top-left corner
  ImGui::Begin("Orbit View", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
               ImGuiWindowFlags_NoCollapse);

  const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();   // Top-left of drawing area
  const ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Size of the window

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(canvas_pos,
  ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
    IM_COL32(10, 10, 10, 255));

  ImGui::SliderFloat("Scale", &inset_scale, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
  const auto origin = ImVec2(canvas_pos.x + canvas_size.x * 0.5f,
                       canvas_pos.y + canvas_size.y * 0.5f);

  for (size_t i = 1; i < body.path_3d.size(); ++i) {
    const glm::vec2 p0 = body.path_3d[i - 1];
    const glm::vec2 p1 = body.path_3d[i];

    auto point0 = ImVec2(origin.x + p0.x / inset_scale, origin.y - p0.y / inset_scale);
    auto point1 = ImVec2(origin.x + p1.x / inset_scale, origin.y - p1.y / inset_scale);

    auto fraction = static_cast<double>(body.path_3d.size() - i) / body.path_3d.size();
    auto alpha = 255 * (1.0 - fraction);

    draw_list->AddLine(point0, point1, IM_COL32(100, 100, 255, alpha), 2.0f);
  }
  draw_list->AddCircleFilled(origin, 2.0f, IM_COL32(255, 255, 0, 255));
  ImGui::End();
}

void GuiHandler::start_main_loop() {
  const auto shape =
      FileLoader::get_shape("../src/obj_files/sphere_centered_scaled.obj");
  const GLuint program_id = load_shaders(
      "../src/shaders/triangle.vert",
      "../src/shaders/triangle.frag");

  SolarSystem solar_system;
  solar_system.init();
  glUseProgram(program_id);

  auto counter = 1;
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
    elapsed_simulation_time += static_cast<float>(delta_time_seconds) / 86400 * simulation_time_factor;


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    done = camera.handle_events(static_cast<float>(delta_time));

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    start_imgui_frame();
    draw_control_window(solar_system);
    draw_2d_overlay(solar_system.bodies[1], inset_scale);
    set_lighting(program_id);

    for (auto& body : solar_system.bodies) {
      glUniform1i(glGetUniformLocation(program_id, "isEmissive"), body.is_emitter ? 1 : 0);
      if (body.is_emitter) {
        light_position = body.position;
      }
      auto model = glm::translate(glm::mat4(1.0f), body.position * 5.0f);
      model = glm::scale(model,
        glm::vec3(static_cast<float>(std::min(body.mass * 100000.0, 0.5))));

      auto mvp = camera.get_vp_matrix(static_cast<float>(window_width),
                                       static_cast<float>(window_height)) * model;
      auto view = camera.get_view_matrix();
      glUniformMatrix4fv(glGetUniformLocation(program_id, "MVP"), 1, GL_FALSE,
                         &mvp[0][0]);
      glUniformMatrix4fv(glGetUniformLocation(program_id, "V"), 1, GL_FALSE,
                         &view[0][0]);
      glUniformMatrix4fv(glGetUniformLocation(program_id, "M"), 1, GL_FALSE,
                         &model[0][0]);
      draw_shape(shape, program_id, body.color);
    }

    if (counter % 2 == 0 && !paused) {
      solar_system.update_bodies_verlet(static_cast<float>(delta_time_seconds) / 60.0f / 60.0f / 24.0f * simulation_time_factor);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
    counter++;
  }
  shutdown();
}
