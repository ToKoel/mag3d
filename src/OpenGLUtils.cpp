//
// Created by Tobias Köhler on 27.05.25.
//

#include "OpenGLUtils.h"

#include <vector>

#include "glm/vec3.hpp"

void OpenGLUtils::bind_array_buffer(const GLuint index, const GLuint buffer_id) {
    glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glVertexAttribPointer(index, 3,GL_FLOAT,GL_FALSE,0, nullptr);
}

void OpenGLUtils::bind_array_buffer_with_data(const GLuint index, const GLuint buffer_id, const std::vector<glm::vec3> &data) {
    bind_array_buffer(index, buffer_id);
    glBufferData(GL_ARRAY_BUFFER,data.size() * sizeof(glm::vec3),
                 data.data(), GL_DYNAMIC_DRAW);
}

Texture OpenGLUtils::setup_texture(const std::string& name, GLuint& texture_id, const std::int32_t& width, const std::int32_t& height, const GLenum color_attachment, const GLenum target) {
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, color_attachment, GL_TEXTURE_2D,texture_id, 0);

    return {.target = target, .position = target - GL_TEXTURE0, .texture_id = texture_id, .name=name};
}

void OpenGLUtils::draw_triangle_faces(const GLsizei number_of_triangles) {
    constexpr int number_of_vertices = 3;
    glDrawArrays(GL_TRIANGLES, 0, number_of_triangles * number_of_vertices);
}


void OpenGLUtils::disable_array_buffer(const GLuint index) {
    glDisableVertexAttribArray(index);
}

void OpenGLUtils::use_main_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLUtils::bind_texture(const GLenum target, const GLuint texture_id) {
    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

