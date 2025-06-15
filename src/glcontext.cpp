#include "../include/glcontext.hpp"


Mesh::~Mesh() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void Mesh::setupMesh() {

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, dims, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Colors
    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, colorSize, colorData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, colordims, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void VirtualScene::addFromMesh(const Mesh& mesh) {
    for (const SceneObject& obj : mesh.scene_objects) {
        addObject(obj);
    }
    scene_object_arrays.back().num_objects++;
}

void VirtualScene::drawScene(GLint *render_as_black) {
        for (const SceneObjectArraytoRender& array : scene_object_arrays) {
            
            for (int i = 0; i < array.num_objects; ++i) {
                const SceneObject& obj = array.objects[i];
                if(array.objects[i].has_color) {
                    glUniform1i(*render_as_black, false); // Disable black mode
                } else {
                    glUniform1i(*render_as_black, true); // Enable black mode
                }
                glLineWidth(obj.line_width);
                
                glDrawElements(
                    obj.rendering_mode,
                    obj.num_indices,
                    GL_UNSIGNED_INT,
                    (void*)obj.first_index
                );
            }
        }
        glBindVertexArray(0);
}
