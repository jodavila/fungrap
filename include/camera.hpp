//Camera related definitions
#include "utils.h"
#include "matrices.h"


#pragma once

#include
class Camera
{
public:
    glm::vec4 position_c; // Posição da câmera no espaço
    glm::vec4 view_vector; // Vetor de visão da câmera
    glm::vec4 up_vector; // Vetor "para cima" da câmera
    bool isOrthographic = false; // Projeção ortográfica ou perspectiva
    float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
    float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y


    Camera();   

    mat4 getProjection ();
protected:
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane = -10.0f; // Posição do "far plane"
    float field_of_view = 3.141592 / 3.0f;
    float screen_ratio = 1.0f
    inline void getScreenRatio(float width, float height) {screen_ratio = width/height}
};

Class Freecam : public Camera
{
    public:
        float speed = 1.0f;
        bool invert_axis = false;

        void move(glm::vec4 direction);

};

Class Lookatcam : public Camera
{
    public:
        float g_CameraDistance = 2.5f; // Distância da câmera para a origem
        glm::vec4 camera_lookat_l = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        void move();
};
