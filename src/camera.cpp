//Camera classes and functions
#include "../include/camera.hpp"
// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"

class Camera
{
public:
    glm::vec4 position_c; // Posição da câmera no espaço
    glm::vec4 view_vector; // Vetor de visão da câmera
    glm::vec4 up_vector; // Vetor "para cima" da câmera

    float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
    float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y


    Camera(glm::vec4 position, glm::vec4 view, glm::vec4 up) : position_c(position), view_vector(view), up_vector(up) {}
};

Class Freecam : public Camera
{
};

Class Lookatcam : public Camera
{
};