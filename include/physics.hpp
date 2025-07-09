//Generic Physics (gravity, projectile motion, etc.)

#pragma once
#include "utils.h"
#include "matrices.hpp"
#define MIN_LIMIAR 0.001f

extern glm::vec4 g; // Gravitational acceleration vector

class RigidBody {
private:
    float mass;          // Mass of the rigid body
    float inertia = 0.5f; // Inertia Momentum of the rigid body, used for angular motion
    float angular_damping = 0.4f; // Damping factor for angular motion
    float linear_damping = 0.4f; // Damping factor for linear motion
    glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // Position in 3D space
    glm::vec4 rotation = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);  // pitch, yaw, roll, w in radians
    glm::vec4 scale = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);     // Scale factors in 3D space
    glm::vec4 velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);  // Linear velocity
    glm::vec4 angular_velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Angular velocity
    glm::vec4 acceleration = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Linear acceleration
    glm::vec4 angular_acceleration = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Angular acceleration
    glm::vec4 force = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Force acting on the body
    glm::vec4 torque = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Torque acting on the body
    glm::vec4 pivot = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Pivot point for rotation
    glm::vec4 center_of_mass = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Center of mass of the rigid body
    public:
    float deltaTime = 0.0f; // Time step for updates

    RigidBody()
        : mass(1.0f)
        , position(0.0f, 0.0f, 0.0f, 1.0f) // Default position
        , rotation(0.0f)
        , scale(1.0f)
        , velocity(0.0f)
        , angular_velocity(0.0f)
        , acceleration(0.0f)
        , angular_acceleration(0.0f)
        , force(0.0f) {}


    RigidBody(float m, glm::vec4 pos, glm::vec4 rot, glm::vec4 scl)
        : mass(m), position(pos), rotation(rot), scale(scl) {}

    RigidBody(glm::vec4 pos, glm::vec4 rot, glm::vec4 scl)
        : mass(1.0f), position(pos), rotation(rot), scale(scl) {}


    void update(float dt);
    // Setters for position, rotation, and scale
    inline void setPosition(glm::vec4 pos) { position = pos; }
    inline void setRotation(glm::vec4 rot) { rotation = rot; }
    inline void setScale(glm::vec4 scl) { scale = scl; }
    inline void setMass(float m) { mass = m; } // Set the mass of the rigid body
    inline void setLinearDamping(float damping) { linear_damping = damping; } // Set the linear damping factor
    inline void setAngularDamping(float damping) { angular_damping = damping; } // Set the angular damping factor
    inline void setVelocity (glm::vec4 vel) {
        velocity = vel; // Set the linear velocity
    }
    inline void setCenterOfMass(glm::vec4 p) { center_of_mass = p; }
    // Apply a force to the rigid body
    inline void setInertia(float i) {
        inertia = i; // Set the inertia of the rigid body
    }
    inline void addForce(glm::vec4 f) {
        force += f; // Add the force to the current force
    }
    inline void setAcceleration(glm::vec4 acc) {
        acceleration = acc; // Set the linear acceleration
    }
    inline void setAngularAcceleration(glm::vec4 acc) {
        angular_acceleration = acc; // Set the angular acceleration
    }
    inline void resetForce() {
        force = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Reset the force to zero
    }
    inline void resetTorque() {
        torque = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Reset the torque to zero
    }
    inline void resetAcceleration() {
        acceleration = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Reset the linear acceleration to zero
    }
    inline void resetVelocity() {
        velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Reset the linear velocity to zero
    }
    inline void resetAngularVelocity() {
        angular_velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Reset the angular velocity to zero
    }
    inline void resetAngularAcceleration() {
        angular_acceleration = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Reset the angular acceleration to zero
    }
    inline void applyTorqueAtPoint(glm::vec4 point, glm::vec4 force_vec) {
        glm::vec4 r = point - pivot; // vetor do centro até o ponto de aplicação
        torque = crossproduct(r, force_vec);
    }
    inline void applyTorque(glm::vec4 torque_vec) {
        torque += torque_vec; // Add the torque to the current torque
    }
    inline float getMass() const { return mass; } // Get the mass of the rigid body
    inline glm::vec4 getPosition() const { return position; } // Get the current position
    inline glm::vec4 getRotation() const { return rotation; } // Get the current rotation
    inline glm::vec4 getScale() const { return scale; } // Get the current scale
    inline glm::vec4 getVelocity() const { return velocity; } // Get the current linear velocity
    inline glm::vec4 getAngularVelocity() const { return angular_velocity; } // Get the current angular velocity
    inline glm::vec4 getAcceleration() const { return acceleration; } // Get the current linear acceleration
    inline glm::vec4 getAngularAcceleration() const { return angular_acceleration; } // Get the current angular acceleration
    inline glm::vec4 getForce() const { return force; } // Get the current
    inline float getLinearDamping() const { return linear_damping; } // Get the linear damping factor
    inline float getAngularDamping() const { return angular_damping; } // Get the
    inline void setPivot(const glm::vec4& p) { pivot = p; }
    inline glm::vec4 getPivot() const { return pivot; }
    glm::vec4 ComputeRigidBodyCenter(const struct ObjModel* model);
    glm::vec4 getFuturePosition() const {
        return (getVelocity() * deltaTime);
    }
    inline float getInertia() const {return inertia;}
    glm::vec4 getCenterOfMass() const { return position + center_of_mass;} // Return the center of the rigid body
};
