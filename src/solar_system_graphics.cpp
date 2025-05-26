
#include "solar_system_graphics.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

void SolarSystemGraphics::init(const int32_t width, const int32_t height) {
    glViewport(0, 0, width, height);
    glGenBuffers(1, &path_vbo);

    glGenFramebuffers(1, &scene_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);

    glGenTextures(1, &non_emissive_texture);
    glBindTexture(GL_TEXTURE_2D,non_emissive_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, non_emissive_texture, 0);

    glGenTextures(1, &emissive_texture);
    glBindTexture(GL_TEXTURE_2D, emissive_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, emissive_texture, 0);

    constexpr GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    glGenRenderbuffers(1, &depth_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete");
}

void SolarSystemGraphics::draw_planets() {
    planet_shader.use();
    planet_shader.setVec3("lightColor", glm::vec3(1.0f));

    for (auto& body : m_calculator.bodies) {
        if (body.is_emitter) {
            light_position = body.draw_position;
        }
        auto model = glm::translate(glm::mat4(1.0f), body.draw_position);
        model = glm::scale(model,
          glm::vec3(static_cast<float>(std::min(body.mass * 50000.0, 0.2))));
        auto mvp = m_camera.get_vp_matrix() * model;
        auto view = m_camera.get_view_matrix();

        planet_shader.setBool("isEmissive", body.is_emitter);
        planet_shader.setMat4("MVP", mvp);
        planet_shader.setMat4("V", view);
        planet_shader.setMat4("M", model);

        constexpr auto number_of_vertices_per_triangle = 3;

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, planet_shape.vertex_buffer_id);
        glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE,0, nullptr);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, planet_shape.normal_buffer_id);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        planet_shader.setVec3("objectColor", body.color);

        glDrawArrays(GL_TRIANGLES, 0, planet_shape.number_of_triangles * number_of_vertices_per_triangle);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
}

bool SolarSystemGraphics::slider_double(const char* label, double& value, const float min, const float max) {
    auto temp = static_cast<float>(value);
    const bool changed = ImGui::SliderFloat(label, &temp, min, max,"%.7f", ImGuiSliderFlags_Logarithmic);
    if (changed) value = static_cast<double>(temp);
    return changed;
}

void SolarSystemGraphics::draw_control_window() const {
        ImGui::Begin("Control");
        slider_double("Sun mass", m_calculator.bodies[0].mass, 0.01f, 100.0f);
        slider_double("Earth mass", m_calculator.bodies[3].mass, 0.0000001f, 1.0f);
        ImGui::SliderFloat("Simulation time factor", &m_calculator.simulation_time_factor, 1.0, 1000000.0, "%.0f", ImGuiSliderFlags_Logarithmic);
        ImGui::Checkbox("Pause", &m_calculator.paused);
        ImGui::Text("Time: %.1f days", m_calculator.elapsed_simulation_time);
        ImGui::End();
}


void SolarSystemGraphics::render_texture() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_camera.window_width, m_camera.window_height);
    texture_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, emissive_texture);
    texture_shader.setInt("emissive_texture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, non_emissive_texture);
    texture_shader.setInt("non_emissive_texture", 1);

    texture_shader.setVec2("tex_size", glm::vec2(m_camera.window_width, m_camera.window_height));

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void SolarSystemGraphics::draw_paths() const {
    path_shader.use();
    const auto vp = m_camera.get_vp_matrix();
    path_shader.setMat4("VP", vp);

    for (const auto& body : m_calculator.bodies) {
        if (body.path_3d.size() < 2) continue;

        std::vector path_vec(body.path_3d.begin(), body.path_3d.end());
        glBindBuffer(GL_ARRAY_BUFFER, path_vbo);
        glBufferData(GL_ARRAY_BUFFER, path_vec.size() * sizeof(glm::vec3),
                     path_vec.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        path_shader.setVec3("objectColor", body.color);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(path_vec.size()));
        glDisableVertexAttribArray(0);
    }
}

void SolarSystemGraphics::draw_solar_system() {
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_planets();
    draw_paths();
    render_texture();
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

        const auto fraction = static_cast<double>(body.path_3d.size() - i) / body.path_3d.size();
        const auto alpha = 255 * (1.0 - fraction);

        draw_list->AddLine(point0, point1, IM_COL32(body.color.r * 255, body.color.g * 255, body.color.b * 255, alpha), 2.0f);
      }
    }
  }
  ImGui::End();
}


