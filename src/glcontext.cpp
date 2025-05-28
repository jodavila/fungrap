#include "../include/glcontext.hpp"


void Mesh::setupMesh(
    const void* vertexData, GLsizeiptr vertexSize, GLint vertexAttribSize,
    const void* colorData, GLsizeiptr colorSize, GLint colorAttribSize,
    const void* indexData, GLsizeiptr indexSize, GLsizei indexCount
) {

    
    this->indexCount = indexCount;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Vértices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, vertexAttribSize, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Cores
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, colorSize, colorData, GL_STATIC_DRAW);
    glVertexAttribPointer(1, colorAttribSize, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

virtual void Mesh::draw(int drawMode) const {
    glBindVertexArray(vao);
    glDrawElements(drawMode, indexCount, GL_UNSIGNED_BYTE, 0);
    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}
