//
// Created by Tobias Köhler on 27.05.25.
//

#pragma once
#include "opengl_utils.h"

class ScopedArrayBuffer {
public:
  ScopedArrayBuffer(const int index, const GLuint buffer_id, const int size) {
    this->index = index;
    OpenGLUtils::bind_array_buffer(index, buffer_id, size);
  }
  ScopedArrayBuffer(const int index, const GLuint buffer_id,
                    const std::vector<glm::vec3> &data) {
    this->index = index;
    OpenGLUtils::bind_array_buffer_with_data(index, buffer_id, data);
  }
  ~ScopedArrayBuffer() { OpenGLUtils::disable_array_buffer(index); }

private:
  int index;
};
