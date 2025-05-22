//
// Created by Tobias Köhler on 20.05.25.
//

#pragma once

#include <iostream>
#include <vector>

#include "glm/glm.hpp"

struct Body {
    glm::vec3 position; // m
    glm::vec3 velocity; // m/s
    double mass; // kg
    glm::vec3 color;
    glm::vec3 force;  // kg * m / s ^ 2
    std::deque<glm::vec2> path_2d;
    bool is_emitter = false;
};

class SolarSystem {
public:
    std::vector<Body> bodies;
    const double G = 6.67430e-6f; // m ^ 3 / kg * s ^2

    void init() {
        float radius = 1.49e11f;
        float mass_sun = 2e30f;
        float mass_earth = 5.972e24f;
        float orbital_velocity = sqrt(G * mass_sun / radius);
        Body sun = { glm::vec3(0), glm::vec3(0), mass_sun, {1.0f, 0.5f, 0.0f} };
        sun.is_emitter = true;
        Body earth = {
            glm::vec3(1.49e11f, 0.0f, 0.0f),
            glm::vec3(0.0, orbital_velocity, 0.0f),
            mass_earth,
            {0.2f, 0.2f, 1.0f}
        };
        bodies.push_back(sun);
        bodies.push_back(earth);
    }

    void computeForces(std::vector<Body>& bodies) const {
        for (auto& b : bodies) b.force = glm::vec3(0.0f);

        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                glm::vec3 r = bodies[j].position - bodies[i].position;
                const double dist = glm::length(r);
                glm::vec3 direction = glm::normalize(r);

                const double force_magnitude = G * bodies[i].mass * bodies[j].mass / (dist * dist);
                const glm::vec3 force = static_cast<float>(force_magnitude) * direction;

                bodies[i].force += force;
                bodies[j].force -= force;
            }
        }
    }

    void updateBodies(const float dt) {
        computeForces(bodies);
        for (auto& body : bodies) {
            glm::vec3 acceleration = body.force / static_cast<float>(body.mass);
            body.velocity += acceleration * dt;
            body.position += body.velocity * dt;
            body.path_2d.emplace_back(body.position / 10e9f);
            while (body.path_2d.size() > 10000) {
                body.path_2d.pop_front();
            }
        }
    }



};



