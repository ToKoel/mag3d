
#include "solar_system_graphics.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

void SolarSystemGraphics::init() {
    planet_shape =
        FileLoader::load_shape("../src/obj_files/sphere_centered_scaled.obj");
    planet_program = FileLoader::load_shaders(
        "../src/shaders/planet.vert",
        "../src/shaders/planet.frag");
    path_program = FileLoader::load_shaders(
      "../src/shaders/path.vert",
      "../src/shaders/path.frag");
    glGenBuffers(1, &path_vbo);
}

void SolarSystemGraphics::draw_planets(glm::vec3 color) {
        constexpr auto number_of_vertices_per_triangle = 3;

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, planet_shape.vertex_buffer_id);
        glVertexAttribPointer(0, // attribute 0. No particular reason for 0, but
                                 // must match the layout in the shader.
                              3, // size
                              GL_FLOAT, // type
                              GL_FALSE, // normalized?
                              0,        // stride
                              nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, planet_shape.normal_buffer_id);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glUniform3f(glGetUniformLocation(planet_program, "objectColor"), color.r, color.g,
                    color.b);

        glDrawArrays(GL_TRIANGLES, 0,
                     planet_shape.number_of_triangles * number_of_vertices_per_triangle);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
}

bool SliderDouble(const char* label, double& value, const float min, const float max) {
    auto temp = static_cast<float>(value);
    const bool changed = ImGui::SliderFloat(label, &temp, min, max,"%.7f", ImGuiSliderFlags_Logarithmic);
    if (changed) value = static_cast<double>(temp);
    return changed;
}

void SolarSystemGraphics::draw_control_window() const {
        ImGui::Begin("Control");
        SliderDouble("Sun mass", m_calculator.bodies[0].mass, 0.01f, 100.0f);
        SliderDouble("Earth mass", m_calculator.bodies[3].mass, 0.0000001f, 1.0f);
        ImGui::SliderFloat("Simulation time factor", &m_calculator.simulation_time_factor, 1.0, 1000000.0, "%.0f", ImGuiSliderFlags_Logarithmic);
        ImGui::Checkbox("Pause", &m_calculator.paused);
        ImGui::Text("Time: %.1f days", m_calculator.elapsed_simulation_time);
        ImGui::End();
}


void SolarSystemGraphics::draw_solar_system() {
    for (auto& body : m_calculator.bodies) {
        glUseProgram(planet_program);
        glUniform1i(glGetUniformLocation(planet_program, "isEmissive"), body.is_emitter ? 1 : 0);
        glUniform3f(glGetUniformLocation(planet_program, "lightColor"), 1.0f, 1.0f, 1.0f);
        if (body.is_emitter) {
            light_position = body.draw_position;
        }
        auto model = glm::translate(glm::mat4(1.0f), body.draw_position);
        model = glm::scale(model,
          glm::vec3(static_cast<float>(std::min(body.mass * 50000.0, 0.2))));

        auto mvp = m_camera.get_vp_matrix() * model;
        auto view = m_camera.get_view_matrix();
        glUniformMatrix4fv(glGetUniformLocation(planet_program, "MVP"), 1, GL_FALSE,
                           &mvp[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(planet_program, "V"), 1, GL_FALSE,
                           &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(planet_program, "M"), 1, GL_FALSE,
                           &model[0][0]);

        draw_planets(body.color);
    }

    glUseProgram(path_program);
    auto mvp = m_camera.get_vp_matrix();
    glUniformMatrix4fv(glGetUniformLocation(path_program, "MVP"), 1, GL_FALSE, &mvp[0][0]);

    for (const auto& body : m_calculator.bodies) {
        if (body.path_3d.size() < 2) continue;

        std::vector path_vec(body.path_3d.begin(), body.path_3d.end());
        glBindBuffer(GL_ARRAY_BUFFER, path_vbo);
        glBufferData(GL_ARRAY_BUFFER, path_vec.size() * sizeof(glm::vec3),
                     path_vec.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glUniform3f(glGetUniformLocation(path_program, "objectColor"), body.color.r, body.color.g,
                    body.color.b);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(path_vec.size()));
        glDisableVertexAttribArray(0);
    }
}

void SolarSystemGraphics::draw_orbit_view() {
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

  for (const auto& body : m_calculator.bodies) {
    if (body.path_3d.size() < 2) continue;
    if (glm::length(body.path_3d.back() - body.path_3d.front()) < 1e-3f) {
      // Draw a dot if the path is too short or stationary
      const glm::vec2 pos = body.path_3d.back();
      auto point = ImVec2(origin.x + pos.x / inset_scale, origin.y - pos.y / inset_scale);
      draw_list->AddCircleFilled(point, 2.0f, IM_COL32(body.color.r * 255, body.color.g * 255, body.color.b * 255, 255));
    } else {
      for (size_t i = 1; i < body.path_3d.size(); ++i) {
        const glm::vec2 p0 = body.path_3d[i - 1];
        const glm::vec2 p1 = body.path_3d[i];

        auto point0 = ImVec2(origin.x + p0.x / inset_scale, origin.y - p0.y / inset_scale);
        auto point1 = ImVec2(origin.x + p1.x / inset_scale, origin.y - p1.y / inset_scale);

        auto fraction = static_cast<double>(body.path_3d.size() - i) / body.path_3d.size();
        auto alpha = 255 * (1.0 - fraction);

        draw_list->AddLine(point0, point1, IM_COL32(body.color.r * 255, body.color.g * 255, body.color.b * 255, alpha), 2.0f);
      }
    }
  }
  ImGui::End();
}


