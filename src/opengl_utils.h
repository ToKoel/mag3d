//
// Created by Tobias Köhler on 27.05.25.
//

#pragma once
#include <string>
#include <OpenGL/gl3.h>

#include "glm/vec3.hpp"


struct Texture {
    GLenum target;
    GLuint position;
    GLuint texture_id;
    std::string name;
};

class OpenGLUtils {
    public:
    static void bind_array_buffer( GLuint index, GLuint buffer_id);
    static void bind_array_buffer_with_data( GLuint index, GLuint buffer_id, const std::vector<glm::vec3> &);
    static Texture setup_texture(const std::string& name, GLuint& texture_id, const std::int32_t& width, const std::int32_t& height, GLenum color_attachment, GLenum target);
    static void draw_triangle_faces(GLsizei number_of_triangles);
    static void disable_array_buffer(GLuint index);
    static void use_main_framebuffer();
    static GLuint create_framebuffer();
    static GLuint create_buffer();
    static void set_viewport(int32_t width, int32_t height);
    static std::vector<GLuint> create_draw_buffers(int32_t);
    static void check_buffer();
    static void bind_frame_buffer(GLuint buffer_id);
    static void clear();
    static void draw_line(const std::vector<glm::vec3>& path_vec);
    static GLuint create_render_buffer(int32_t width, int32_t height);
    static void bind_texture(GLenum target, GLuint texture_id);
};



