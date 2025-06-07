#pragma once
#include "solar_system_calculator.h"
#include <OpenGL/gl3.h>
#include <string>

#include "camera.hpp"
#include "file_loader.h"
#include "opengl_utils.h"
#include "shader.h"


class SolarSystemGraphics {
    SolarSystemCalculator& m_calculator;
    Camera& m_camera;
    Body* m_selected_body = nullptr;
    std::vector<Texture> textures;
    const std::string planet_fragment_shader_path = "../src/shaders/planet.frag";
    const std::string planet_vertex_shader_path = "../src/shaders/planet.vert";
    const std::string path_fragment_shader_path = "../src/shaders/path.frag";
    const std::string path_vertex_shader_path = "../src/shaders/path.vert";
    const std::string planet_shape_path = "../src/obj_files/sphere_auto_smooth.obj";
    const std::string passthrough_vertex_shader_path = "../src/shaders/passthrough.vert";
    const std::string texture_fragment_shader_path = "../src/shaders/texture.frag";
    const std::string blur_shader_path = "../src/shaders/gaussian_blur.frag";
    const std::string threshold_shader_path = "../src/shaders/threshold.frag";

    Shader planet_shader{planet_vertex_shader_path, planet_fragment_shader_path};
    Shader path_shader{path_vertex_shader_path, path_fragment_shader_path};
    Shader texture_shader{passthrough_vertex_shader_path, texture_fragment_shader_path};
    Shader blur_shader{passthrough_vertex_shader_path, blur_shader_path};
    Shader threshold_shader{passthrough_vertex_shader_path, threshold_shader_path};

    Shape planet_shape = FileLoader::load_shape(planet_shape_path);

    GLuint path_vbo = 0;
    GLuint path_fbo = 0;
    GLuint path_texture = 0;
    GLuint scene_fbo = 0;
    GLuint scene_texture = 0;
    GLuint threshold_texture = 0;
    GLuint depth_render_buffer = 0;
    GLuint pingpongFBO[2];
    GLuint pingpongColorbuffers[2];
    GLuint threshold_fbo = 0;

    float inset_scale{0.015};


    glm::vec3 light_position{0.0};

    void draw_planets(GLuint);
    void draw_paths();
    void render_texture() const;
    void check_selection();
    void render_info() const;
    void apply_bloom_blur() const;
    void create_threshold_texture() const;

    static bool slider_double(const char* label, double& value, float min, float max);

    public:
    bool show_paths{false};
    bool enable_bloom{false};
    float bloom_strength{1.0f};
    float bloom_threshold{0.6f};
    SolarSystemGraphics(SolarSystemCalculator& calculator, Camera& camera) : m_calculator(calculator), m_camera(camera) {};
    void init(int32_t, int32_t);
    void draw_control_window();
    void draw_solar_system();
    void draw_orbit_view();


};



