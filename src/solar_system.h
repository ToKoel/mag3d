//
// Created by Tobias Köhler on 20.05.25.
//

#pragma once

#include <iostream>
#include <vector>

#include "glm/glm.hpp"

struct Body {
    glm::vec3 position; // m
    glm::vec3 draw_position;
    glm::vec3 velocity; // m/s
    double mass; // kg
    glm::vec3 color;
    glm::vec3 force;  // kg * m / s ^ 2
    std::deque<glm::vec3> path_3d;
    bool is_emitter = false;
    glm::vec3 prev_acceleration;
};

struct BodyParameters {
    double mass;
    double radius;
};

class SolarSystem {
public:
    std::vector<Body> bodies;
    const double G = 6.67430e-6f; // m ^ 3 / kg * s ^2
    const double G_au = 2.96e-4; // au^3 / m_s day^2
    const double gamma = 1.327e25; // m^3 s^-2
    const double AU = 1.49e11; // m
    const double MU = 5.972e24f; // kg
    const float position_scale = 5.0f;

    void init() {
        Body sun = {
            .position = glm::vec3(0),
            .velocity = glm::vec3(0),
            .mass = 1.0,
            .color = {1.0f, 0.5f, 0.0f},
            .is_emitter = true
        };
        Body earth = {
            .position = glm::vec3(1.0, 0.0f, 0.0f),
            .velocity = glm::vec3(0.0, 0.017199389, 0.0f),
            .mass = 2e-6,
            .color = {0.2f, 0.2f, 1.0f}
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

                const double force_magnitude = G_au * bodies[i].mass * bodies[j].mass / (dist * dist);
                const glm::vec3 force = static_cast<float>(force_magnitude) * direction;

                bodies[i].force += force;
                bodies[j].force -= force;
            }
        }
    }

    void update_bodies_verlet(const float dt) {
        computeForces(bodies);

        for (auto& body : bodies) {
            glm::vec3 acceleration = body.force / static_cast<float>(body.mass);

            // Store current acceleration for velocity update later
            body.prev_acceleration = acceleration;

            // Update position
            body.position += body.velocity * dt + 0.5f * acceleration * dt * dt;
        }

        // Recompute forces after updating positions
        computeForces(bodies);

        for (auto& body : bodies) {
            glm::vec3 new_acceleration = body.force / static_cast<float>(body.mass);

            // Update velocity using average of old and new acceleration
            body.velocity += 0.5f * (body.prev_acceleration + new_acceleration) * dt;

            body.draw_position = body.position * position_scale;
            body.path_3d.emplace_back(body.draw_position);
            while (body.path_3d.size() > 10000) {
                body.path_3d.pop_front();
            }
        }
    }

};



