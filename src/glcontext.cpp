#include "../include/glcontext.hpp"

void Mesh::setupMesh() {

    GLsizeiptr vertexSize = sizeof(vertexData);
    GLsizeiptr colorSize = sizeof(colorData);
    GLsizeiptr indexSize = sizeof(indexData);


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Vértices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, dims, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Cores
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, colorSize, colorData, GL_STATIC_DRAW);
    glVertexAttribPointer(1, colordims, GL_FLOAT, GL_FALSE, 0, 0);
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

void Mesh::draw(int drawMode) const {
    glBindVertexArray(vao);
    glDrawElements(drawMode, indexCount, GL_UNSIGNED_BYTE, 0);
    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

void VirtualScene::addFromMesh(const Mesh& mesh) {
    for (const SceneObject& obj : mesh.scene_objects) {
        addObject(obj);
    }
    scene_object_arrays.back().num_objects++;
}
