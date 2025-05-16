//Camera classes and functions
#include "../include/camera.hpp"

void Camera::Camera() : position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), up_vector(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)) {}

void Camera::getProjection() {
    if (isOrthographic) {
        float t = 1.5f * camera_distance / 2.0f;
        float b = -t;
        float r = t * getScreenRatio(width, height);
        float l = -r;
    }
    else {
        return Matrix_Perspective(field_of_view, screen_ratio, nearplane, farplane);
    }
}
void Camera::setCameraAxis() {
    if (invert_yaxis) {
        camera_axis.y = -1.0f;
    }
    else {
        camera_axis.y = 1.0f;
    }
    if (invert_xaxis) {
        camera_axis.x = -1.0f;
    }
    else {
        camera_axis.x = 1.0f;
    }
}
void Camera::setCameraAngles() {
        float yaw = sin(g_CameraPhi);
        float roll = cos(g_CameraPhi) * cos(g_CameraTheta);
        float pitch = cos(g_CameraPhi) * sin(g_CameraTheta);
}
void Freecam::move() {
    glm::vec4 u, v, w;
    w = -camera_view_vector;
    w = w / norm(w);
    u = crossproduct(camera_up_vector, w);
    u = u / norm(u);
    v = crossproduct(w, u);
    v = v / norm(v);

    Timer frameTime;
    float deltaTime = frameTime.getFrameTime();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

        position += -w * speed * (float)(deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

        position += w * speed * (float)(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {

        position += -u * speed * (float)(deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

        position += u * speed * (float)(deltaTime);
    }

}

void Freecam::getVectors() {
    camera_view_vector = glm::vec4(pitch, yaw, roll, 0.0f); //Inserir aqui logica para converter de coordenadas esfericas para cartesianas camera phi e camera theta
    camera_view_vector = camera_view_vector / norm(camera_view_vector);

}
