
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

    [[nodiscard]] std::string toString() const {
        return std::to_string(position.x) + " " +
            std::to_string(position.y) + " " +
                std::to_string(position.z);
    }

    [[nodiscard]] size_t hash() const {
        return std::hash<std::string>()(toString());
    }

    bool operator==(const ObjVertex& other) const {
        return position == other.position && normal == other.normal && uvs == other.uvs;
    }
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



