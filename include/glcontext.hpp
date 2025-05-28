#ifndef _GL_CONTEXT_HPP
#define _GL_CONTEXT_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>

// We define a structure that will store the necessary data to render
// each object in the virtual scene.
struct SceneObject {
    const char* name;        // Object name
    void* first_index; // Index of the first vertex within the indices[] vector defined in BuildTriangles()
    int          num_indices; // Number of indices of the object within the indices[] vector defined in BuildTriangles()
    GLenum       rendering_mode; // Rasterization mode (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    bool has_color = true; // Indicates if the object has an associated color
    GLfloat line_width = 4.0f; // Line width for objects of type GL_LINES
};

struct SceneObjectArraytoRender {
    SceneObject* objects; // Pointer to the array of objects
    int num_objects;      // Number of objects in the array
};

class Mesh {
protected:
    GLuint vao = 0; // Vertex Array Object
    // The VAO is an object that contains the IDs of the vertex and index buffers
    GLuint vbo = 0; // Vertex Buffer Object
    // The VBO is a memory buffer that contains the vertex data
    GLuint ebo = 0; // Element Buffer Object
    // The EBO is a memory buffer that contains the vertex indices

    std::vector<GLfloat> vertexData; // Vertex data as a vector
    std::vector<GLfloat> colorData; // Color data as a vector
    std::vector<GLuint> indexData; // Index data as a vector
    GLsizei vertexSize = 0; // Size of the vertex data
    GLsizei colorSize = 0; // Size of the color data
    GLsizei indexSize = 0; // Size of the index data
    GLint dims = 4; // (x, y, z, w)
    GLint colordims = 4; // (r, g, b, a)

    void setupMesh();

public:
    Mesh() = default;
    virtual ~Mesh();
    std::vector<SceneObject> scene_objects;
    inline GLuint getVAO() const { return vao; }
};

class VirtualScene {
public:
    std::vector<SceneObjectArraytoRender> scene_object_arrays; // array of arrays of SceneObjects to render
    VirtualScene() = default;
    ~VirtualScene() {
        clear();
    }	
    inline void clear() {
        for (SceneObjectArraytoRender& array : scene_object_arrays) {
            delete[] array.objects;
        }
        scene_object_arrays.clear();
    }

    void drawScene(GLint *render_as_black);

    void addFromMesh(const Mesh& mesh);

private:
    inline void addObject(const SceneObject& obj) {
        scene_object_arrays.push_back(
            SceneObjectArraytoRender{
                new SceneObject[1]{obj}, // Creates a new array with the object
                1 // Sets the number of objects to 1
            }
        );
    }
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
