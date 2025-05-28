#ifndef _GL_CONTEXT_HPP
#define _GL_CONTEXT_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>


// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject {
    const char* name;        // Nome do objeto
    void* first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTriangles()
    int          num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTriangles()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    bool has_color = true; // Indica se o objeto tem cor associada
};

struct SceneObjectArraytoRender {
    SceneObject* objects; // Ponteiro para o array de objetos
    int num_objects;      // Número de objetos no array
};

class Mesh {
protected:
    GLuint vao = 0; // Vertex Array Object
    // O VAO é um objeto que contém os IDs dos buffers de vértices e de índices
    GLuint vbo = 0; // Vertex Buffer Object
    // O VBO é um buffer de memória que contém os dados dos vértices
    GLuint ebo = 0; // Element Buffer Object
    // O EBO é um buffer de memória que contém os índices dos vértices
    GLsizei indexCount = 0; // Número de índices a serem desenhados
    const void* vertexData = nullptr; // Dados dos vértices
    const void* colorData = nullptr; // Dados das cores
    const void* indexData = nullptr; // Dados dos índices
    GLint dims = 4; // (x, y, z, w)
    GLint colordims = 4; // (r, g, b, a)


    void setupMesh();


public:
    Mesh() = default;
    virtual ~Mesh();
    void draw(int drawMode) const;
    std::vector<SceneObject> scene_objects;
};


class VirtualScene {
public:
    std::vector<SceneObjectArraytoRender> scene_object_arrays; //array of arrays of SceneObjects to render
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

    inline void DrawScene(GLint *render_as_black) {
        for (const SceneObjectArraytoRender& array : scene_object_arrays) {
            for (int i = 0; i < array.num_objects; ++i) {
                const SceneObject& obj = array.objects[i];
                if(array.objects[i].has_color) {
                    glUniform1i(*render_as_black, false); // Desliga o modo preto
                } else {
                    glUniform1i(*render_as_black, true); // Liga o modo preto
                }
                glDrawElements(
                    obj.rendering_mode,
                    obj.num_indices,
                    GL_UNSIGNED_INT,
                    (void*)obj.first_index
                );
            }
        }
    }

    void addFromMesh(const Mesh& mesh);

private:
    inline void addObject(const SceneObject& obj) {
        scene_object_arrays.push_back(
            SceneObjectArraytoRender{
                new SceneObject[1]{obj}, // Cria um novo array com o objeto
                1 // Define o número de objetos como 1
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