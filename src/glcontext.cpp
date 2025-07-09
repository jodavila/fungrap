#include "../include/glcontext.hpp"


void VirtualScene::drawAll(GLuint programID) {
    for (auto const& object : scene_objects) {
      // desenha cada um
      glUniform1i(glGetUniformLocation(programID, "render_as_black"), object.second.has_color ? 0 : 1);
      glBindVertexArray(object.second.vao);
      glDrawElements(
        object.second.rendering_mode,
        object.second.num_indices,
        GL_UNSIGNED_INT,
        (void*)(object.second.first_index * sizeof(GLuint))
      );
    }
    glBindVertexArray(0);
  }

  void VirtualScene::draw(GLuint programID, const std::string& objectName) {
    auto it = scene_objects.find(objectName);
    if (it != scene_objects.end()) {
        const SceneObject& object = it->second;

        glUniform1i(glGetUniformLocation(programID, "render_as_black"), object.has_color ? 0 : 1);
        glBindVertexArray(object.vao);
        glDrawElements(
            object.rendering_mode,
            object.num_indices,
            GL_UNSIGNED_INT,
            (void*)(object.first_index * sizeof(GLuint))
        );
        glBindVertexArray(0);
        
    } else {
        throw std::runtime_error("Object not found in the virtual scene: " + objectName);
    }
  }
  