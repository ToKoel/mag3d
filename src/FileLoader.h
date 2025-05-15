//
// Created by Tobias Köhler on 14.05.25.
//

#pragma once
#include <string>
#include <vector>
#include <span>

#include <OpenGL/gl3.h>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

struct ObjShape {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
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
    static ObjShape load_obj_file(const std::string& path);
    static Shape get_shape(const std::string& path);

};



