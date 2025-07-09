//Camera classes and functions
#include "../include/camera.hpp"
#include "../include/timer.hpp"

float g_CameraTheta = 0.0f; // Angle in the ZX plane relative to the Z axis
float g_CameraPhi = 0.0f;   // Angle relative to the Y axis
float camera_distance = 2.5f; // Distance from the camera to the origin
glm::vec2 camera_axis = glm::vec2(1.0f, 1.0f); // Invert or not the camera axis
bool g_LeftMouseButtonPressed = false;
bool invert_yaxis = false;
bool invert_xaxis = false;

Camera::Camera() : position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), up_vector(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) {}

glm::mat4 Camera::getProjection() {
    if (isOrthographic) {
        float t = 1.5f * camera_distance / 2.0f;
        float b = -t;
        float r = t * getScreenRatio(width, height);
        float l = -r;

        return Matrix_Orthographic(l, r, b, t, nearplane, farplane);
    }
    else {

        return Matrix_Perspective(field_of_view, screen_ratio, nearplane, farplane);
    }
}
void Camera::setCameraAxis() {
    if (invert_yaxis) {
        camera_axis.y = 1.0f;
    }
    else {
        camera_axis.y = -1.0f;
    }
    if (invert_xaxis) {
        camera_axis.x = -1.0f;
    }
    else {
        camera_axis.x = 1.0f;
    }
}

void Camera::setCameraAngles() {
    yaw = sin(g_CameraPhi);
    roll = cos(g_CameraPhi) * cos(g_CameraTheta);
    pitch = cos(g_CameraPhi) * sin(g_CameraTheta);
}

void Camera::move(GLFWwindow* window, double deltaTime) {
    glm::vec4 u, v, w;
    w = -view_vector;
    if (norm(w))
        w = w / norm(w);

    u = crossproduct(up_vector, w);

    if (norm(u))
        u = u / norm(u);

    v = crossproduct(w, u);

    if (norm(v))
        v = v / norm(v);

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
    view_vector = glm::vec4(pitch, yaw, roll, 0.0f); // Insert here logic to convert from spherical to cartesian coordinates using camera phi and camera theta
    if (norm(view_vector))
        view_vector = view_vector / norm(view_vector);
}


Lookatcam::Lookatcam(float distance, glm::vec4 lookat_l) : Camera() {
    camera_distance = distance;
    view_vector = lookat_l - position; // Vector "view", direction where the camera is looking
  
}



