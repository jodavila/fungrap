//Collisions tests classes and functions

#include "../include/collisions.hpp"
#include "../include/geometrics.hpp"
#include "../include/utils.h"
#include "glm/gtx/string_cast.hpp" // For glm::to_string
#include <iostream>

bool collisor::SphereToPlane(Ball &ball, Plane &plane) {
    glm::vec4 ball_center = ball.getCenter() + ball.body->getFuturePosition(); // Future position of the ball
    glm::vec4 plane_center = plane.getCenter();  
    float epsilon = 0.1f; // Tolerance for floating point comparison

    // Normal do plano
    glm::vec4 plane_normal = plane.normal;

    // Distância do centro da esfera ao plano
    float distance = glm::dot(ball_center - plane_center, plane_normal);
    // Verifica se a distância é menor ou igual ao raio da esfera
    return std::abs(distance) <= ball.radius + epsilon; // Use epsilon to avoid floating point precision issues
}

bool collisor::SphereToCube(Ball& ball, Cube& cube) {
    float epsilon = 0.01f; // ou zero, se quiser precisão exata

// 1) Centro da bola (mundo)
glm::vec4 ball_center_world = ball.getCenter(); // w = 1

// 2) Transformação do cubo
glm::mat4 cube_transform = cube.transform;
glm::mat4 cube_inv_transform = glm::inverse(cube_transform);

// 3) Centro da bola no espaço local do cubo
glm::vec4 ball_local = cube_inv_transform * ball_center_world;

// 4) Metade das dimensões
float hw = cube.width * 0.5f;
float hh = cube.height * 0.5f;
float hd = cube.depth * 0.5f;

// 5) Ponto mais próximo no AABB local
float cx = glm::clamp(ball_local.x, -hw, hw);
float cy = glm::clamp(ball_local.y, -hh, hh);
float cz = glm::clamp(ball_local.z, -hd, hd);
glm::vec4 closest_local = glm::vec4(cx, cy, cz, 1.0f);

// 6) Transforma de volta para o mundo
glm::vec4 closest_world = cube_transform * closest_local;

// 7) Vetor do ponto mais próximo até o centro da bola
glm::vec4 d = ball_center_world - closest_world;
d.w = 0.0f;
float dist = glm::length(d);

// 8) Atualiza normal se houver contato
if (dist > 0.0001f)
    cube.normal = glm::normalize(d);
else
    cube.normal = glm::vec4(0, 1, 0, 0); // fallback

// 9) Teste de colisão
return dist <= (ball.radius + epsilon);

}


//Test if the sphere is inside the radius of the cylinder
bool collisor::SphereToCylinder(Ball &ball, Cylinder &cylinder) {
    glm::vec4 ball_center = ball.getCenter();
    glm::vec4 cylinder_center = cylinder.getCenter();
    // Calculate the distance from the ball's center to the cylinder's center in the XZ plane
    float distance = glm::length(glm::vec3(ball_center.x, 0.0f, ball_center.z) - glm::vec3(cylinder_center.x, 0.0f, cylinder_center.z));
    return distance <= (ball.radius + cylinder.radius);
}

//Check if the ball enters in the hole
bool collisor::SphereToCylinderBottom(Ball &ball, Cylinder &cylinder) {
    collisor col;
    //Check if the ball is inside the radius of the cylinder
    if(col.SphereToCylinder(ball, cylinder)) {
        glm::vec4 ball_center = ball.getCenter();
        glm::vec4 cylinder_center = cylinder.getCenter();
        // Check if the ball is below the cylinder's bottom
        if(ball_center.y < cylinder_center.y - cylinder.height / 2.0f) {
            // Check if the ball is within the radius of the cylinder
            float distance = glm::length(glm::vec3(ball_center.x, 0.0f, ball_center.z) - glm::vec3(cylinder_center.x, 0.0f, cylinder_center.z));
            return distance <= cylinder.radius;
        }
    }
    return false;
}
bool collisor::SphereToSphere(Ball &ball1, Ball &ball2) {
    glm::vec4 center1 = ball1.getCenter();
    glm::vec4 center2 = ball2.getCenter();
    return glm::length(center1 - center2) <= (ball1.radius + ball2.radius);

}
