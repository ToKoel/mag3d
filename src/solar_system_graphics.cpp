
#include "solar_system_graphics.h"

#include "glm/glm.hpp"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

#include "opengl_utils.h"
#include "scoped_array_buffer.h"

void SolarSystemGraphics::init(const int32_t width, const int32_t height) {
    OpenGLUtils::set_viewport(width, height);

    path_vbo = OpenGLUtils::create_buffer();
    scene_fbo = OpenGLUtils::create_framebuffer();

    const auto draw_buffers = OpenGLUtils::create_draw_buffers(2);
    textures.push_back(OpenGLUtils::setup_texture("non_emissive_texture", non_emissive_texture, width, height,
                                                  draw_buffers[0], GL_TEXTURE0));
    textures.push_back(OpenGLUtils::setup_texture("emissive_texture", emissive_texture, width, height, draw_buffers[1],
                                                  GL_TEXTURE1));

    depth_render_buffer = OpenGLUtils::create_render_buffer(width, height);
    OpenGLUtils::check_buffer();
}

bool ray_sphere_intersect(const glm::vec3 ray_origin, const glm::vec3 ray_dir, const glm::vec3 sphere_center,
                          const float radius) {
    const glm::vec3 oc = ray_origin - sphere_center;
    const float a = glm::dot(ray_dir, ray_dir); // usually 1.0
    const float b = 2.0f * glm::dot(oc, ray_dir);
    const float c = glm::dot(oc, oc) - radius * radius;

    const float discriminant = b * b - 4.0f * a * c;
    return discriminant >= 0.0f;
}

void SolarSystemGraphics::check_selection() {
    if (m_camera.current_mouse_ray != glm::vec3(0.0)) {
        bool found = false;
        for (auto &body: m_calculator.bodies) {
            if (ray_sphere_intersect(m_camera.position, m_camera.current_mouse_ray, body.draw_position, 0.2)) {
                m_selected_body = &body;
                found = true;
            }
        }
        if (!found) {
            m_selected_body = nullptr;
        }
    }
    m_camera.current_mouse_ray = glm::vec3(0.0);
}


void SolarSystemGraphics::draw_planets() {
    planet_shader.use();
    planet_shader.setVec3("lightColor", glm::vec3(1.0f));

    const auto view = m_camera.get_view_matrix();
    const auto vp = m_camera.get_vp_matrix();

    for (auto &body: m_calculator.bodies) {
        if (body.is_emitter) {
            light_position = body.draw_position;
        }

        auto model = glm::translate(glm::mat4(1.0f), body.draw_position);
        model = glm::scale(model,
                           glm::vec3(static_cast<float>(std::min(body.mass * 50000.0, 0.2))));
        auto mvp = vp * model;

        planet_shader.setBool("isEmissive", body.is_emitter);
        planet_shader.setMat4("MVP", mvp);
        planet_shader.setMat4("V", view);
        planet_shader.setMat4("M", model);
        planet_shader.setBool("selected", &body == m_selected_body);
        planet_shader.setVec3("objectColor", body.color);

        ScopedArrayBuffer vertex_buffer{0, planet_shape.vertex_buffer_id};
        ScopedArrayBuffer normal_buffer{1, planet_shape.normal_buffer_id};

        OpenGLUtils::draw_triangle_faces(planet_shape.number_of_triangles);
    }
}

bool SolarSystemGraphics::slider_double(const char *label, double &value, const float min, const float max) {
    auto temp = static_cast<float>(value);
    const bool changed = ImGui::SliderFloat(label, &temp, min, max, "%.7f", ImGuiSliderFlags_Logarithmic);
    if (changed) value = static_cast<double>(temp);
    return changed;
}

void SolarSystemGraphics::draw_control_window() const {
    ImGui::Begin("Control");
    slider_double("Sun mass", m_calculator.bodies[0].mass, 0.01f, 100.0f);
    slider_double("Earth mass", m_calculator.bodies[3].mass, 0.0000001f, 1.0f);
    ImGui::SliderFloat("Simulation time factor", &m_calculator.simulation_time_factor, 1.0, 1000000.0, "%.0f",
                       ImGuiSliderFlags_Logarithmic);
    ImGui::Checkbox("Pause", &m_calculator.paused);
    ImGui::Text("Time: %.1f days", m_calculator.elapsed_simulation_time);
    ImGui::End();
}

void SolarSystemGraphics::render_texture() const {
    OpenGLUtils::use_main_framebuffer();
    texture_shader.use();

    for (const auto &[target, position, texture_id, name]: textures) {
        OpenGLUtils::bind_texture(target, texture_id);
        texture_shader.setInt(name, position);
    }

    texture_shader.setVec2("tex_size", glm::vec2(m_camera.window_width, m_camera.window_height));
    OpenGLUtils::draw_triangle_faces(1);
}

void SolarSystemGraphics::draw_paths() const {
    path_shader.use();
    const auto vp = m_camera.get_vp_matrix();
    path_shader.setMat4("VP", vp);

    for (const auto &body: m_calculator.bodies) {
        if (body.path_3d.size() < 2) continue;
        path_shader.setVec3("objectColor", body.color);

        std::vector path_vec(body.path_3d.begin(), body.path_3d.end());
        ScopedArrayBuffer path{0, path_vbo, path_vec};
        OpenGLUtils::draw_line(path_vec);
    }
}

void SolarSystemGraphics::render_info() const {
    if (!m_selected_body) return;

    ImGui::Begin("Planet info");
    ImGui::Text("Name: %s", m_selected_body->name.c_str());
    ImGui::Text("Distance from sun (AU): %.2f", glm::length(m_selected_body->position));
    ImGui::Text("Orbital velocity (AU/day): %.5f, %.5f, %.5f", m_selected_body->velocity.x, m_selected_body->velocity.y,
                m_selected_body->velocity.z);
    ImGui::End();
}

void SolarSystemGraphics::draw_solar_system() {
    OpenGLUtils::bind_frame_buffer(scene_fbo);
    OpenGLUtils::clear();

    check_selection();
    draw_planets();
    draw_paths();
    render_texture();
    render_info();
}

void SolarSystemGraphics::draw_orbit_view() {
    ImGui::SetNextWindowSize(ImVec2(200, 200));
    ImGui::SetNextWindowPos(ImVec2(10, 10)); // top-left corner
    ImGui::Begin("Orbit View", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse);

    const ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // Top-left of drawing area
    const ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Size of the window

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_pos,
                             ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                             IM_COL32(10, 10, 10, 255));

    ImGui::SliderFloat("Scale", &inset_scale, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
    const auto origin = ImVec2(canvas_pos.x + canvas_size.x * 0.5f,
                               canvas_pos.y + canvas_size.y * 0.5f);

    for (const auto &body: m_calculator.bodies) {
        if (body.path_3d.size() < 2) continue;
        if (glm::length(body.path_3d.back() - body.path_3d.front()) < 1e-3f) {
            // Draw a dot if the path is too short or stationary
            const glm::vec2 pos = body.path_3d.back();
            auto point = ImVec2(origin.x + pos.x / inset_scale, origin.y - pos.y / inset_scale);
            draw_list->AddCircleFilled(point, 2.0f,
                                       IM_COL32(body.color.r * 255, body.color.g * 255, body.color.b * 255, 255));
        } else {
            for (size_t i = 1; i < body.path_3d.size(); ++i) {
                const glm::vec2 p0 = body.path_3d[i - 1];
                const glm::vec2 p1 = body.path_3d[i];

                auto point0 = ImVec2(origin.x + p0.x / inset_scale, origin.y - p0.y / inset_scale);
                auto point1 = ImVec2(origin.x + p1.x / inset_scale, origin.y - p1.y / inset_scale);

                const auto fraction = static_cast<double>(body.path_3d.size() - i) / body.path_3d.size();
                const auto alpha = 255 * (1.0 - fraction);

                draw_list->AddLine(point0, point1,
                                   IM_COL32(body.color.r * 255, body.color.g * 255, body.color.b * 255, alpha), 2.0f);
            }
        }
    }
    ImGui::End();
}


