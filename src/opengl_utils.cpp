//
// Created by Tobias Köhler on 27.05.25.
//

#include "opengl_utils.h"

#include <vector>

#include "glm/vec3.hpp"

void OpenGLUtils::bind_array_buffer(const GLuint index, const GLuint buffer_id) {
    glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glVertexAttribPointer(index, 3,GL_FLOAT,GL_FALSE,0, nullptr);
}

void OpenGLUtils::bind_frame_buffer(const GLuint buffer_id) {
    glBindFramebuffer(GL_FRAMEBUFFER, buffer_id);
    clear();
}

void OpenGLUtils::set_viewport(const int32_t width, const int32_t height) {
    glViewport(0, 0, width, height);
}

void OpenGLUtils::bind_array_buffer_with_data(const GLuint index, const GLuint buffer_id, const std::vector<glm::vec3> &data) {
    bind_array_buffer(index, buffer_id);
    glBufferData(GL_ARRAY_BUFFER,data.size() * sizeof(glm::vec3),
                 data.data(), GL_DYNAMIC_DRAW);
}

GLuint OpenGLUtils::create_render_buffer(const int32_t width, const int32_t height) {
    GLuint buffer_id;
    glGenRenderbuffers(1, &buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,buffer_id);
    return buffer_id;
}

void OpenGLUtils::check_buffer() {
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer is not complete");
}

std::vector<GLuint> OpenGLUtils::create_draw_buffers(const int32_t number_of_buffers) {
    std::vector<GLuint> buffer_ids(number_of_buffers);
    for (int i = 0; i < number_of_buffers; i++) {
        buffer_ids[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(number_of_buffers, buffer_ids.data());
    return buffer_ids;
}

GLuint OpenGLUtils::create_buffer() {
    GLuint buffer_id;
    glGenBuffers(1, &buffer_id);
    return buffer_id;
}

GLuint OpenGLUtils::create_framebuffer(const int32_t width, const int32_t height) {
    GLuint framebuffer_id;
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    create_render_buffer(width, height);
    check_buffer();
    return framebuffer_id;
}

GLuint OpenGLUtils::setup_texture(const std::int32_t& width, const std::int32_t& height) {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,texture_id, 0);

    return texture_id;
}

void OpenGLUtils::draw_triangle_faces(const GLsizei number_of_triangles) {
    constexpr int number_of_vertices = 3;
    glDrawArrays(GL_TRIANGLES, 0, number_of_triangles * number_of_vertices);
}

void OpenGLUtils::draw_line(const std::vector<glm::vec3>& path_vec) {
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(path_vec.size()));
}


void OpenGLUtils::disable_array_buffer(const GLuint index) {
    glDisableVertexAttribArray(index);
}

void OpenGLUtils::use_main_framebuffer() {
    bind_frame_buffer(0);
}

void OpenGLUtils::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLUtils::bind_texture(const GLenum target, const GLuint texture_id) {
    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

