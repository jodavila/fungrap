//Classical Mechanics Physics Simulation
#include <iostream>
#include <vector>
#include "../include/physics.hpp"
#include "../include/matrices.hpp"
#include "../include/geometrics.hpp"

glm::vec4 g = glm::vec4(0.0f, -9.81f, 0.0f, 0.0f); // Gravitational acceleration in m/s^2

void RigidBody::update(float dt) {
    deltaTime = dt; // Update the time step

    acceleration = (force / mass); // Update acceleration based on force and mass
    angular_acceleration = (torque / inertia); // Update angular acceleration based on torque and mass
    float tmp = angular_acceleration.x;
    angular_acceleration.x = angular_acceleration.z;
    angular_acceleration.z = -tmp; // Swap x and z components for rotation
    angular_acceleration.y = -angular_acceleration.y; // Invert y component for correct rotation direction

    force = {0.0f, 0.0f, 0.0f, 0.0f}; // Reset forces after applying them
    torque = {0.0f, 0.0f, 0.0f, 0.0f}; // Reset torque after applying them

    velocity += acceleration * dt;

    velocity *= (1.0f - linear_damping * dt);

 

    position += velocity * dt;

    // Update rotation based on angular velocity
    angular_velocity += angular_acceleration * dt;
    angular_velocity *= (1.0f - angular_damping * dt); // Apply angular damping


        
    rotation += angular_velocity * dt;

}

glm::vec4 RigidBody::ComputeRigidBodyCenter(const struct ObjModel* model) {
    const std::vector<float>& vertices = model->attrib.vertices;

    if (vertices.empty())
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // fallback

    glm::vec4 sum(0.0f);
    size_t count = vertices.size() / 3;

    for (size_t i = 0; i < vertices.size(); i += 3) {
        glm::vec4 v(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
        sum += v;
    }
    
    return sum / static_cast<float>(count); // Centro geométrico

}

