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

    resetForce(); // Reset the force after applying it
    resetTorque(); // Reset the torque after applying it
    
    if (norm(force) < MIN_LIMIAR)
        resetForce(); // Reset force if it's below the threshold
    if (norm(torque) < MIN_LIMIAR)
        resetTorque(); // Reset torque if it's below the threshold
    
    if (norm(acceleration) < MIN_LIMIAR)
        resetAcceleration(); // Reset acceleration if it's below the threshold
    velocity += acceleration * dt;

    velocity *= (1.0f - linear_damping * dt);

    if (norm(velocity) < MIN_LIMIAR)
        resetVelocity(); // Reset velocity if it's below the threshold


    position += velocity * dt;

    // Update rotation based on angular velocity
    angular_velocity += angular_acceleration * dt;
    angular_velocity *= (1.0f - angular_damping * dt); // Apply angular damping

    if (glm::length(angular_velocity) < MIN_LIMIAR)
        resetAngularVelocity(); // Reset angular velocity if it's below the threshold


        
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

