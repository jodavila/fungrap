
#ifndef _CAMERA_H
#define _CAMERA_H


//Camera related definitions
#include "matrices.hpp"
#include "GLFW/glfw3.h"  // Criação de janelas do sistema operacional

//Theses variables are declared global to be used by the cursor Callback function
extern float g_CameraTheta; // Ângulo no plano ZX em relação ao eixo Z
extern float g_CameraPhi;   // Ângulo em relação ao eixo Y
extern float camera_distance; // Distância da câmera para a origem
extern glm::vec2 camera_axis; //invert or not the camera axis
extern bool g_LeftMouseButtonPressed;
extern bool invert_yaxis;
extern bool invert_xaxis;


class Camera {
public:
    glm::vec4 position; // Posição da câmera no espaço
    glm::vec4 view_vector; // Vetor de visão da câmera
    glm::vec4 up_vector; // Vetor "para cima" da câmera
    bool isOrthographic = false; // Projeção ortográfica ou perspectiva


    Camera();

    glm::mat4 getProjection();
    void setCameraAxis();
    void setCameraAngles();
    inline glm::mat4 getView() {return Matrix_Camera_View(position, view_vector, up_vector);}
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

    inline float getScreenRatio(float width, float height) {
        return width / height;
    }
    
    
};

class Freecam : public Camera
{
    public:
        float speed = 2.5f;
        void move(GLFWwindow* window, double deltaTime);
        void getVectors();

};

class Lookatcam : public Camera
{
    public:
        float camera_distance = 2.5f; // Distância da câmera para a origem
        glm::vec4 camera_lookat_l = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        void move();
};


#endif // _CAMERA_H