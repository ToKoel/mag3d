#include "FileLoader.h"

#include <fstream>
#include <iostream>
#include <ostream>
#include <ranges>
#include <sstream>
#include <unordered_set>

std::vector<ObjVertex> FileLoader::load_obj_file(std::string_view path) {
  std::cout << "Loading obj file " << path << "\n";

  auto file = std::ifstream{};

  file.open(path.data(), std::ios::in | std::ios::binary);
  if (file.fail())
    throw std::runtime_error("Could not open file");

  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
  std::unordered_set<size_t> vertices_hashes;
  std::vector<ObjVertex> vertices;
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream ss(line);
    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      glm::vec3 pos;
      ss >> pos.x >> pos.y >> pos.z;
      positions.push_back(pos);
    } else if (prefix == "vt") {
      glm::vec2 uv;
      ss >> uv.x >> uv.y;
      uvs.push_back(uv);
    } else if (prefix == "vn") {
      glm::vec3 norm;
      ss >> norm.x >> norm.y >> norm.z;
      normals.push_back(norm);
    } else if (prefix == "f") {
      std::array<std::string, 3> face_parts;
      ss >> face_parts[0] >> face_parts[1] >> face_parts[2];

      for (const auto &part : face_parts) {
        std::istringstream part_stream(part);
        std::string idx_str;
        std::array indices = {0, 0, 0};

        size_t i = 0;
        while (std::getline(part_stream, idx_str, '/')) {
          if (!idx_str.empty()) {
            indices[i] = std::stoi(idx_str);
          }
          ++i;
        }

        ObjVertex vertex{};
        vertex.position = positions.at(indices[0] - 1);
        if (indices[1] > 0 && indices[1] <= uvs.size())
          vertex.uvs = uvs.at(indices[1] - 1);
        if (indices[2] > 0 && indices[2] <= normals.size())
          vertex.normal = normals.at(indices[2] - 1);

        if (!vertices_hashes.contains(indices[0])) {
          vertices_hashes.insert(vertex.hash());
          vertices.push_back(vertex);
        }
      }
    }
  }
  return vertices;
}

std::pair<GLuint, GLuint>
FileLoader::init_shape(const std::span<glm::vec3> vertex_buffer_data,
                       const std::span<glm::vec3> normal_buffer_data) {
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  GLuint vertex_array_id;
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  GLuint vertex_buffer_id;
  glGenBuffers(1, &vertex_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizeiptr>(sizeof(glm::vec3) * vertex_buffer_data.size()),
      vertex_buffer_data.data(), GL_STATIC_DRAW);

  GLuint normal_buffer_id;
  glGenBuffers(1, &normal_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizeiptr>(sizeof(glm::vec3) * normal_buffer_data.size()),
      normal_buffer_data.data(), GL_STATIC_DRAW);

  return {vertex_buffer_id, normal_buffer_id};
}

Shape FileLoader::get_shape(const std::string &path) {
  const auto vertices = load_obj_file(path);
  std::vector<glm::vec3> positions(vertices.size());
  std::ranges::transform(vertices, positions.begin(),
                         [](const ObjVertex &v) { return v.position; });
  std::vector<glm::vec3> normals(vertices.size());
  std::ranges::transform(vertices, normals.begin(),
                         [](const ObjVertex &v) { return v.normal; });

  auto [vertex_buffer_id, normal_buffer_id] = init_shape(positions, normals);
  return {static_cast<GLsizei>(vertices.size()), vertex_buffer_id,
          normal_buffer_id};
}
