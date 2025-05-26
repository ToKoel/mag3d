#pragma once
#include "solar_system_calculator.h"
#include <OpenGL/gl3.h>
#include <string>

#include "camera.hpp"
#include "file_loader.h"
#include "shader.h"


class SolarSystemGraphics {
    SolarSystemCalculator& m_calculator;
    Camera& m_camera;
    const std::string planet_fragment_shader_path = "../src/shaders/planet.frag";
    const std::string planet_vertex_shader_path = "../src/shaders/planet.vert";
    const std::string path_fragment_shader_path = "../src/shaders/path.frag";
    const std::string path_vertex_shader_path = "../src/shaders/path.vert";
    const std::string planet_shape_path = "../src/obj_files/sphere_centered_scaled.obj";

    Shader planet_shader{planet_vertex_shader_path, planet_fragment_shader_path};
    Shader path_shader{path_vertex_shader_path, path_fragment_shader_path};
    Shader texture_shader{"../src/shaders/passthrough.vert", "../src/shaders/texture.frag"};

    Shape planet_shape;

    GLuint path_vbo;
    GLuint quad_vbo;
    GLuint quad_vao;
    GLuint scene_fbo;
    GLuint non_emissive_texture;
    GLuint emissive_texture;
    GLuint depth_render_buffer;

    float inset_scale{0.015};

    glm::vec3 light_position{0.0};

    void draw_planets(glm::vec3 color) const;
    void draw_paths();
    void render_texture() const;

    public:
    SolarSystemGraphics(SolarSystemCalculator& calculator, Camera& camera) : m_calculator(calculator), m_camera(camera) {};
    void init(int32_t, int32_t);
    void draw_control_window() const;
    void draw_solar_system();
    void draw_orbit_view();


};



