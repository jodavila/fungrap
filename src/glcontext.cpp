#include "../include/glcontext.hpp"

void VirtualScene::draw(GLint& render_as_black, std::string object_name) {

    glBindVertexArray(this->scene_objects[object_name].vao);

    glDrawElements(
        this->scene_objects[object_name].rendering_mode,
        this->scene_objects[object_name].num_indices,
        GL_UNSIGNED_INT,
        (void*)(this->scene_objects[object_name].first_index * sizeof(GLuint))
    );
    glBindVertexArray(0);

}
