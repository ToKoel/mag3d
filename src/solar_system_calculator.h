//
// Created by Tobias Köhler on 20.05.25.
//

#pragma once

#include <iostream>
#include <vector>

#include "glm/glm.hpp"

struct Body {
  glm::vec3 position; // au
  glm::vec3 draw_position;
  glm::vec3 velocity; // au/day
  double mass;        // m_sun
  glm::vec3 color;
  glm::vec3 force;
  std::deque<glm::vec3> path_3d;
  bool is_emitter = false;
  glm::vec3 prev_acceleration;
  const std::size_t max_path = 5000;
  std::string name;
};

class SolarSystemCalculator {
public:
  std::vector<Body> bodies;
  const float position_scale = 2.0f;
  double elapsed_simulation_time = 0.0;
  float simulation_time_factor = 1000.0;
  bool paused = false;
  bool show_paths = false;

  void init();
  void update_bodies_verlet(float dt);

private:
  const double G = 2.96e-4;   // au^3 / m_s day^2
  void compute_forces();
};
