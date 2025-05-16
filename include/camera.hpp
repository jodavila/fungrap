//Camera related definitions
#include "utils.h"
#include "matrices.h"


#pragma once

//Theses variables are declared global to be used by the cursor Callback function
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 2.5f; // Distância da câmera para a origem
glm::vec2 camera_axis = glm::vec2(1.0f, 1.0f); //invert or not the camera axis
bool g_LeftMouseButtonPressed = false;
bool invert_yaxis = false;
bool invert_xaxis = false;


class Camera {
public:
    glm::vec4 position; // Posição da câmera no espaço
    glm::vec4 view_vector; // Vetor de visão da câmera
    glm::vec4 up_vector; // Vetor "para cima" da câmera
    bool isOrthographic = false; // Projeção ortográfica ou perspectiva


    Camera();

    mat4 getProjection();
protected:
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane = -10.0f; // Posição do "far plane"
    float field_of_view = 3.141592 / 3.0f;
    float screen_ratio = 1.0f;
    float width = 800.0f; // screen width
    float height = 800.0f; // screen height

    float yaw = 0.0f; // rotation around the Y axis
    float pitch = 0.0f; // rotation around the X axis
    float roll = 0.0f; // rotation around the Z axis
    void setCameraAxis();
    inline float getScreenRatio(float width, float height) {
        return width / height;
    }
    void setCameraAngles();
    inline glm::mat4 getView() return Matrix_Camera_View(position, camera_view_vector, camera_up_vector);
};

Class Freecam : public Camera
{
    public:
        float speed = 1.0f;
        void move();
        void getVectors();

};

Class Lookatcam : public Camera
{
    public:
        float g_CameraDistance = 2.5f; // Distância da câmera para a origem
        glm::vec4 camera_lookat_l = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        void move();
};
