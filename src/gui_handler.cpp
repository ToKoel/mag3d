#include "gui_handler.hpp"
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <imgui_impl_opengl3_loader.h>

#include <array>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <span>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "load_shader.hpp"
#include <SDL_opengl.h>

#include "FileLoader.h"

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
}

void GuiHandler::draw_control_window(glm::vec3 *rotation) {
  ImGui::Begin("Control");
  ImGui::SliderFloat("Rotate X", &rotation->x, -180.0f, 180.0f);
  ImGui::SliderFloat("Rotate Y", &rotation->y, -180.0f, 180.0f);
  ImGui::SliderFloat("Rotate Z", &rotation->z, -180.0f, 180.0f);
  ImGui::DragFloat3("Light Position", glm::value_ptr(light_position), 0.1f);
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

void GuiHandler::handle_events(const SDL_Event *event) {
  if (event->type == SDL_MOUSEWHEEL) {
    camera.update_field_of_view(event->wheel.y);
  }

  if (event->type == SDL_QUIT)
    done = true;

  if (event->type == SDL_MOUSEBUTTONDOWN &&
      event->button.button == SDL_BUTTON_LEFT) {
    if (!io->WantCaptureMouse) {
      dragging = true;
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }
  if (event->type == SDL_MOUSEBUTTONUP &&
      event->button.button == SDL_BUTTON_LEFT) {
    dragging = false;
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }

  if (event->type == SDL_MOUSEMOTION && dragging) {
    camera.update_camera_directions(static_cast<float>(event->motion.xrel),
                                    static_cast<float>(event->motion.yrel),
                                    static_cast<float>(delta_time));
  }
}

void draw_shape(const Shape shape, const GLuint program_id) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(program_id);

  const GLint color = glGetUniformLocation(program_id, "objectColor");
  glUniform3f(color, 1.0f, 0.0f, 0.0f); // Red

  // 1st attribute buffer : vertices
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

  // Draw the triangles !
  glDrawArrays(GL_TRIANGLES, 0,
               shape.number_of_triangles * 3); // 3 vertices per triangle
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

void GuiHandler::start_main_loop() {
  const auto shape =
      FileLoader::get_shape("/Users/tobiaskohler/Documents/projects/"
                            "magnetic/src/obj_files/arrow.obj");
  GLuint program_id = load_shaders(
      "/Users/tobiaskohler/Documents/projects/magnetic/src/shaders/"
      "triangle.vert",
      "/Users/tobiaskohler/Documents/projects/magnetic/src/shaders/"
      "triangle.frag");

  SDL_Event event;

  glm::vec3 model_rotation = glm::vec3(0.0f); // x, y, z rotation in degrees

  while (!done) {
    last_time = now_time;
    now_time = SDL_GetPerformanceCounter();
    delta_time = static_cast<double>((now_time - last_time) * 1000) /
                 static_cast<double>(SDL_GetPerformanceFrequency());

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      handle_events(&event);
    }

    const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
    camera.update_camera_position(keyboardState,
                                  static_cast<float>(delta_time));

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    draw_control_window(&model_rotation);

    glUniform3f(glGetUniformLocation(program_id, "objectColor"), 1.0f, 0.0f,
                0.0f);
    glUniform3f(glGetUniformLocation(program_id, "lightColor"), 1.0f, 1.0f,
                1.0f);
    glUniform3fv(glGetUniformLocation(program_id, "lightPos"), 1,
                 glm::value_ptr(light_position));

    auto model = glm::mat4(1.0f);
    model =
        glm::rotate(model, glm::radians(model_rotation.x), glm::vec3(1, 0, 0));
    model =
        glm::rotate(model, glm::radians(model_rotation.y), glm::vec3(0, 1, 0));
    model =
        glm::rotate(model, glm::radians(model_rotation.z), glm::vec3(0, 0, 1));

    auto mvp = camera.get_mvp_matrix(static_cast<float>(window_width),
                                     static_cast<float>(window_height), model);
    auto view = camera.get_view_matrix();
    glUniformMatrix4fv(glGetUniformLocation(program_id, "MVP"), 1, GL_FALSE,
                       &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program_id, "V"), 1, GL_FALSE,
                       &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program_id, "M"), 1, GL_FALSE,
                       &model[0][0]);
    draw_shape(shape, program_id);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  shutdown();
}
