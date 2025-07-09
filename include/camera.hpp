
#ifndef _CAMERA_H
#define _CAMERA_H


//Camera related definitions
#include "matrices.hpp"
#include "utils.h"
extern float g_CameraTheta; // Ângulo no plano ZX em relação ao eixo Z
extern float g_CameraPhi;   // Ângulo em relação ao eixo Y
extern float camera_distance; // Distância da câmera para a origem
extern glm::vec2 camera_axis; //invert or not the camera axis
extern bool g_LeftMouseButtonPressed;
extern bool invert_yaxis;
extern bool invert_xaxis;


class Camera {
public:
    glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Posição da câmera no espaço
    glm::vec4 view_vector; // Vetor de visão da câmera
    glm::vec4 up_vector; // Vetor "para cima" da câmera
    bool isOrthographic = false; // Projeção ortográfica ou perspectiva
    float yaw = 0.0f; // rotation around the Y axis
    float pitch = 0.0f; // rotation around the X axis
    float roll = 0.0f; // rotation around the Z axis


    Camera();

    glm::mat4 getProjection();
    void setCameraAxis();
    void setCameraAngles();
    inline glm::vec4 getCameraAngles() {
        return glm::vec4(pitch, yaw, roll, 1.0f);
    }
    inline glm::mat4 getView() { return Matrix_Camera_View(position, view_vector, up_vector); }
    inline void sendToGPU(GLuint view_program_id, GLuint projection_program_id) {
        glUniformMatrix4fv(view_program_id, 1, GL_FALSE, glm::value_ptr(getView()));
        glUniformMatrix4fv(projection_program_id, 1, GL_FALSE, glm::value_ptr(getProjection()));
    }
    inline void setPosition(glm::vec4 pos) { position = pos; }
    inline glm::vec4 getPosition() { return position; }
    inline glm::vec4 getViewVector()    { return view_vector; }
    inline glm::vec4 getRotation() {
        return glm::vec4(pitch, yaw, roll, 1.0f);
    }
    float speed = 6.0f;
    void move(GLFWwindow* window, double deltaTime);
protected:
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane = -200.0f; // Posição do "far plane"
    float field_of_view = 3.141592 / 3.0f;
    float screen_ratio = 1.0f;
    float width = 800.0f; // screen width
    float height = 800.0f; // screen height

    inline float getScreenRatio(float width, float height) {
        return width / height;
    }



};

class Freecam : public Camera {
public:
    
    void getVectors();

};

class Lookatcam : public Camera {
public:
    float camera_distance = 10.0f; // Distância da câmera para a origem
    Lookatcam() = default;
    Lookatcam(float distance, glm::vec4 lookat_l);
    inline void setView(glm::vec4 lookat_l) {
        view_vector = lookat_l - position;
       /*  if (norm(view_vector))
            view_vector = view_vector / norm(view_vector); // Normalize */
    }


};


#endif // _CAMERA_H