#include "solar_system_calculator.h"

void SolarSystemCalculator::init() {
  const std::unordered_map<std::string, float> orbit_inclinations{
      {"Mercury", glm::radians(7.004f)}, {"Venus", glm::radians(3.395f)},
      {"Earth", glm::radians(0.0f)},     {"Moon", glm::radians(5.145f)},
      {"Mars", glm::radians(1.848f)},
  };
  const Body sun = {.position = glm::vec3(0),
                    .velocity = glm::vec3(0),
                    .mass = 1.0,
                    .color = {1.0f, 0.5f, 0.0f},
                    .max_path = 10,
                    .is_emitter = true,
                    .name = "Sun"};
  const Body mercury = {
      .position = glm::vec3(0.39, 0.0f, 0.0f),
      .velocity =
          glm::vec3(0.0, 0.027352689f * cos(orbit_inclinations.at("Mercury")),
                    0.027352689f * sin(orbit_inclinations.at("Mercury"))),
      .mass = 1.1e-7,
      .color = {0.678f, 0.6588f, 0.647f},
      .max_path = 2000,
      .name = "Mercury"};
  const Body venus = {
      .position = glm::vec3(0.72, 0.0f, 0.0f),
      .velocity =
          glm::vec3(0.0f, 0.020225742 * cos(orbit_inclinations.at("Venus")),
                    0.020225742 * sin(orbit_inclinations.at("Venus"))),
      .mass = 1.63e-6,
      .color = {0.7568f, 0.56078f, 0.0901f},
      .name = "Venus"};
  const Body earth = {
      .position = glm::vec3(1.0, 0.0f, 0.0f),
      .velocity =
          glm::vec3(0.0, 0.017199389 * cos(orbit_inclinations.at("Earth")),
                    0.017199389 * sin(orbit_inclinations.at("Earth"))),
      .mass = 2e-6,
      .color = {0.4196f, 0.57647f, 0.83921f},
      .name = "Earth"};
  const Body mars = {.position = glm::vec3(1.5, 0.0f, 0.0f),
                     .velocity = glm::vec3(
                         0.0, 0.0139056311 * cos(orbit_inclinations.at("Mars")),
                         0.0139056311 * sin(orbit_inclinations.at("Mars"))),
                     .mass = 3.213e-7,
                     .color = {0.757f, 0.27f, 0.0549f},
                     .max_path = 10000,
                     .name = "Mars"};
  bodies.push_back(sun);
  bodies.push_back(mercury);
  bodies.push_back(venus);
  bodies.push_back(earth);
  bodies.push_back(mars);
}

void SolarSystemCalculator::compute_forces() {
  for (auto &b : bodies)
    b.force = glm::vec3(0.0f);

  for (size_t i = 0; i < bodies.size(); ++i) {
    for (size_t j = i + 1; j < bodies.size(); ++j) {
      glm::vec3 r = bodies[j].position - bodies[i].position;
      const double dist = glm::length(r);
      glm::vec3 direction = glm::normalize(r);

      const double force_magnitude =
          G * bodies[i].mass * bodies[j].mass / (dist * dist);
      const glm::vec3 force = static_cast<float>(force_magnitude) * direction;

      bodies[i].force += force;
      bodies[j].force -= force;
    }
  }
}

void SolarSystemCalculator::update_bodies_verlet(const float dt) {
  compute_forces();

  for (auto &body : bodies) {
    glm::vec3 acceleration = body.force / static_cast<float>(body.mass);

    body.prev_acceleration = acceleration;
    body.position += body.velocity * dt + 0.5f * acceleration * dt * dt;
  }

  compute_forces();

  for (auto &body : bodies) {
    glm::vec3 new_acceleration = body.force / static_cast<float>(body.mass);

    body.velocity += 0.5f * (body.prev_acceleration + new_acceleration) * dt;
    body.draw_position = body.position * position_scale;
    body.path_3d.emplace_back(body.draw_position);
    while (body.path_3d.size() > body.max_path) {
      body.path_3d.pop_front();
    }
  }
}
