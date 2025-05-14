//
// Created by Tobias Köhler on 14.05.25.
//

#pragma once
#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

struct ObjShape {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
};

class FileLoader {
public:
    static ObjShape load_obj_file(const std::string& path);

};



