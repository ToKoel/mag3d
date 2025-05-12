#include "gui_handler.hpp"

#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <imgui_impl_opengl3_loader.h>
#include <stdio.h>

#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "load_shader.hpp"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

bool GuiHandler::init() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    printf("Error: %s\n", SDL_GetError());
    return false;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100 (WebGL 1.0)
  const char* glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  const char* glsl_version = "#version 300 es";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_FLAGS,
      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);  // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                        SDL_WINDOW_ALLOW_HIGHDPI);

  window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            window_width, window_height, window_flags);
  if (window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return false;
  }

  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
    return false;
  }

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = &ImGui::GetIO();
  (void)io;
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return true;
}

void draw_control_window(float* angle_x, float* angle_y) {
  ImGui::Begin("Control");
  ImGui::Text("Use this window to control rotation");
  ImGui::SliderFloat("Rotate X", angle_x, 0.0f, 360.0f);
  ImGui::SliderFloat("Rotate Y", angle_y, 0.0f, 360.0f);

  ImGui::End();
}

glm::mat4 GuiHandler::get_view_matrix() {
  glm::mat4 projection = glm::perspective(
      glm::radians(field_of_view),
      static_cast<float32_t>(window_width / window_height), 0.1f, 100.0f);

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

void GuiHandler::shutdown() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void GuiHandler::update_camera_directions(float deltaX, float deltaY) {
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

void GuiHandler::handle_events(SDL_Event* event) {
  bool dragging = false;

  while (SDL_PollEvent(event)) {
    ImGui_ImplSDL2_ProcessEvent(event);
    switch (event->type) {
      std::cout << event->type << "\n";
      case SDL_MOUSEWHEEL:
        if (event->wheel.y > 0) {
          field_of_view *= 1.1f;  // Zoom in
        } else if (event->wheel.y < 0) {
          field_of_view /= 1.1f;  // Zoom out
        }
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        switch (event->key.keysym.sym) {
          case SDLK_LEFT:
            position -= right * (float)delta_time * speed;
            break;
          case SDLK_RIGHT:
            position += right * (float)delta_time * speed;
            break;
          case SDLK_UP:
            position += direction * (float)delta_time * speed;
            break;
          case SDLK_DOWN:
            position -= direction * (float)delta_time * speed;
            break;
          default:
            break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event->button.button == SDL_BUTTON_LEFT) {
          SDL_SetRelativeMouseMode(SDL_TRUE);
          dragging = true;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event->button.button == SDL_BUTTON_LEFT) {
          SDL_SetRelativeMouseMode(SDL_FALSE);
          dragging = false;
        }
        break;
      case SDL_MOUSEMOTION:
        if (dragging) {
          update_camera_directions(event->motion.xrel, event->motion.yrel);
        }
        break;
      case SDL_QUIT:
      case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_CLOSE &&
            event->window.windowID == SDL_GetWindowID(window)) {
          done = true;
        }
        break;
      default:
        break;
    }
  }
}

template <std::size_t N, std::size_t K>
std::pair<GLuint, GLuint> init_shape(
    const std::array<GLfloat, N>& vertex_buffer_data,
    const std::array<GLfloat, K>& color_buffer_data) {
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // This will identify our vertex buffer
  GLuint vertexbuffer;
  // Generate 1 buffer, put the resulting identifier in vertexbuffer
  glGenBuffers(1, &vertexbuffer);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  // Give our vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * N, vertex_buffer_data.data(),
               GL_STATIC_DRAW);

  GLuint colorbuffer;
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * N, color_buffer_data.data(),
               GL_STATIC_DRAW);

  return std::pair{vertexbuffer, colorbuffer};
}

std::pair<GLuint, GLuint> init_triangle() {
  static const std::array<GLfloat, 9U> g_vertex_buffer_data = {
      -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };
  static const std::array<GLfloat, 27U> g_color_buffer_data = {
      0.583f, 0.771f, 0.014f, 0.609f, 0.115f, 0.436f, 0.327f, 0.483f, 0.844f,
      0.822f, 0.569f, 0.201f, 0.435f, 0.602f, 0.223f, 0.310f, 0.747f, 0.185f,
      0.597f, 0.770f, 0.761f, 0.559f, 0.436f, 0.730f, 0.359f, 0.583f, 0.152f};

  return init_shape(g_vertex_buffer_data, g_color_buffer_data);
}

std::pair<GLuint, GLuint> init_cube() {
  static const std::array<GLfloat, 128U> g_vertex_buffer_data = {
      -1.0f, -1.0f, -1.0f,                       // triangle 1 : begin
      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,   // triangle 1 : end
      1.0f,  1.0f,  -1.0f,                       // triangle 2 : begin
      -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,  // triangle 2 : end
      1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f};

  static const std::array<GLfloat, 384> g_color_buffer_data = {
      0.583f, 0.771f, 0.014f, 0.609f, 0.115f, 0.436f, 0.327f, 0.483f, 0.844f,
      0.822f, 0.569f, 0.201f, 0.435f, 0.602f, 0.223f, 0.310f, 0.747f, 0.185f,
      0.597f, 0.770f, 0.761f, 0.559f, 0.436f, 0.730f, 0.359f, 0.583f, 0.152f,
      0.483f, 0.596f, 0.789f, 0.559f, 0.861f, 0.639f, 0.195f, 0.548f, 0.859f,
      0.014f, 0.184f, 0.576f, 0.771f, 0.328f, 0.970f, 0.406f, 0.615f, 0.116f,
      0.676f, 0.977f, 0.133f, 0.971f, 0.572f, 0.833f, 0.140f, 0.616f, 0.489f,
      0.997f, 0.513f, 0.064f, 0.945f, 0.719f, 0.592f, 0.543f, 0.021f, 0.978f,
      0.279f, 0.317f, 0.505f, 0.167f, 0.620f, 0.077f, 0.347f, 0.857f, 0.137f,
      0.055f, 0.953f, 0.042f, 0.714f, 0.505f, 0.345f, 0.783f, 0.290f, 0.734f,
      0.722f, 0.645f, 0.174f, 0.302f, 0.455f, 0.848f, 0.225f, 0.587f, 0.040f,
      0.517f, 0.713f, 0.338f, 0.053f, 0.959f, 0.120f, 0.393f, 0.621f, 0.362f,
      0.673f, 0.211f, 0.457f, 0.820f, 0.883f, 0.371f, 0.982f, 0.099f, 0.879f};

  return init_shape(g_vertex_buffer_data, g_color_buffer_data);
}

void draw_shape(GLuint vertexbuffer, GLuint colorbuffer, GLuint programId,
                size_t number_of_triangles) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(programId);

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0,  // attribute 0. No particular reason for 0, but
                            // must match the layout in the shader.
                        3,  // size
                        GL_FLOAT,  // type
                        GL_FALSE,  // normalized?
                        0,         // stride
                        (void*)0   // array buffer offset
  );

  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(1,         // attribute. No particular reason for 1, but
                                   // must match the layout in the shader.
                        3,         // size
                        GL_FLOAT,  // type
                        GL_FALSE,  // normalized?
                        0,         // stride
                        (void*)0   // array buffer offset
  );

  // Draw the triangles !
  glDrawArrays(
      GL_TRIANGLES, 0,
      number_of_triangles *
          3);  // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
}

void GuiHandler::start_main_loop() {
  float rotation = 0.0f;
  float angle_x = 0.0f, angle_y = 0.0f;
  bool rotate = true;

  std::pair<GLuint, GLuint> buffer_ids = init_cube();
  GLuint program_id = load_shaders(
      "/Users/tobiaskohler/Documents/projects/magnetic/src/shaders/"
      "triangle.vert",
      "/Users/tobiaskohler/Documents/projects/magnetic/src/shaders/"
      "triangle.frag");

  GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

  bool dragging = false;
  SDL_Event event;

  while (!done) {
    last_time = now_time;
    now_time = SDL_GetPerformanceCounter();
    delta_time =
        ((now_time - last_time) * 1000 / (double)SDL_GetPerformanceFrequency());
    // handle_events(&event);

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
          field_of_view -= 1.0f;
        }
        if (event.wheel.y < 0) {
          field_of_view += 1.0f;
        }

        if (field_of_view < 1.0f) field_of_view = 1.0f;
        if (field_of_view > 90.0f) field_of_view = 90.0f;
      }

      if (event.type == SDL_QUIT) done = true;

      if (event.type == SDL_MOUSEBUTTONDOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
        dragging = true;
        SDL_SetRelativeMouseMode(SDL_TRUE);
      }
      if (event.type == SDL_MOUSEBUTTONUP &&
          event.button.button == SDL_BUTTON_LEFT) {
        dragging = false;
        SDL_SetRelativeMouseMode(SDL_FALSE);
      }

      if (event.type == SDL_MOUSEMOTION && dragging) {
        update_camera_directions(event.motion.xrel, event.motion.yrel);
      }
    }

    // --- 2. Handle Keyboard State ---
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_RIGHT]) {
      position += right * speed * (float)delta_time;
    }
    if (keyboardState[SDL_SCANCODE_LEFT]) {
      position -= right * speed * (float)delta_time;
    }
    if (keyboardState[SDL_SCANCODE_UP]) {
      position += up * speed * (float)delta_time;
    }
    if (keyboardState[SDL_SCANCODE_DOWN]) {
      position -= up * speed * (float)delta_time;
    }

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    draw_control_window(&vertical_angle, &horizontal_angle);

    draw_shape(buffer_ids.first, buffer_ids.second, program_id, 12);

    auto mvp = get_view_matrix();
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }
  shutdown();
}
