
#pragma once
#include <string>
#include <vector>
#include <span>

#include <OpenGL/gl3.h>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

struct ObjVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uvs;
};

struct Shape {
    GLsizei number_of_triangles;
    GLuint vertex_buffer_id;
    GLuint normal_buffer_id;
};

class FileLoader {
    static std::pair<GLuint, GLuint> init_shape(std::span<glm::vec3> vertex_buffer_data,
               std::span<glm::vec3> normal_buffer_data);
        public:
    static std::vector<ObjVertex> load_obj_file(std::string_view path);
    static Shape get_shape(const std::string& path);

};



