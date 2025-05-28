#ifndef _GL_CONTEXT_HPP
#define _GL_CONTEXT_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>


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

    void setupMesh(
        , GLint vertexAttribSize,
        const void* colorData, GLsizeiptr colorSize, GLint colorAttribSize,
        const void* indexData, GLsizeiptr indexSize, GLsizei indexCount
    ) {}

public:
    Mesh() = default;
    virtual ~Mesh() {}

    virtual void draw(int drawMode) const {}

};


#endif // _GL_CONTEXT_HPP