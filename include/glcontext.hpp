#ifndef _GL_CONTEXT_HPP
#define _GL_CONTEXT_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "tiny_obj_loader.h"
#include "utils.h"
#include "matrices.hpp"
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>


struct SceneObject {
    std::string name;        // Object name
    size_t first_index; // Index of the first vertex within the indices[] vector defined in BuildTriangles()
    int num_indices; // Number of indices of the object within the indices[] vector defined in BuildTriangles()
    GLuint vao; // ID of the VAO where the model attributes are stored
    GLenum rendering_mode; // Rasterization mode (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    bool has_color = true; // Indicates if the object has an associated color
    GLfloat line_width = 4.0f; // Line width for objects of type GL_LINES
};

class VirtualScene {
private:
    int id_count = 0;
public:
    std::map<std::string, SceneObject> scene_objects; // Map of objects in the virtual scene

    VirtualScene() = default;
    ~VirtualScene() = default;
    inline int getNextId() { return id_count++; }
    void drawAll(GLuint program_id);
    void draw(GLuint program_id, const std::string& object_name);
};

class Callback {
public:
    virtual ~Callback() {}
    virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
    virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) = 0;
    virtual void onCursorPos(GLFWwindow* window, double xpos, double ypos) = 0;
    virtual void onScroll(GLFWwindow* window, double xoffset, double yoffset) = 0;
};


#endif // _GL_CONTEXT_HPP
