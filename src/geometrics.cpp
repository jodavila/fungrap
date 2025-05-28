#include "../include/geometrics.hpp"
#include <vector>

Cube::Cube() {

    vertexData = {
         //    X      Y     Z     W
            -0.5f,  0.5f,  0.5f, 1.0f, // vertex 0 position
            -0.5f, -0.5f,  0.5f, 1.0f, // vertex 1 position
             0.5f, -0.5f,  0.5f, 1.0f, // vertex 2 position
             0.5f,  0.5f,  0.5f, 1.0f, // vertex 3 position
            -0.5f,  0.5f, -0.5f, 1.0f, // vertex 4 position
            -0.5f, -0.5f, -0.5f, 1.0f, // vertex 5 position
             0.5f, -0.5f, -0.5f, 1.0f, // vertex 6 position
             0.5f,  0.5f, -0.5f, 1.0f, // vertex 7 position
             // Vertices to draw the X axis
             //    X      Y     Z     W
                  0.0f,  0.0f,  0.0f, 1.0f, // vertex 8 position
                  1.0f,  0.0f,  0.0f, 1.0f, // vertex 9 position
                  // Vertices to draw the Y axis
                  //    X      Y     Z     W
                       0.0f,  0.0f,  0.0f, 1.0f, // vertex 10 position
                       0.0f,  1.0f,  0.0f, 1.0f, // vertex 11 position
                       // Vertices to draw the Z axis
                       //    X      Y     Z     W
                            0.0f,  0.0f,  0.0f, 1.0f, // vertex 12 position
                            0.0f,  0.0f,  1.0f, 1.0f, // vertex 13 position
    };
    colorData = {
        // Colors of the cube vertices
        //  R     G     B     A
            1.0f, 0.5f, 0.0f, 1.0f, // color of vertex 0
            1.0f, 0.5f, 0.0f, 1.0f, // color of vertex 1
            0.0f, 0.5f, 1.0f, 1.0f, // color of vertex 2
            0.0f, 0.5f, 1.0f, 1.0f, // color of vertex 3
            1.0f, 0.5f, 0.0f, 1.0f, // color of vertex 4
            1.0f, 0.5f, 0.0f, 1.0f, // color of vertex 5
            0.0f, 0.5f, 1.0f, 1.0f, // color of vertex 6
            0.0f, 0.5f, 1.0f, 1.0f, // color of vertex 7
            // Colors to draw the X axis
                1.0f, 0.0f, 0.0f, 1.0f, // color of vertex 8
                1.0f, 0.0f, 0.0f, 1.0f, // color of vertex 9
                // Colors to draw the Y axis
                    0.0f, 1.0f, 0.0f, 1.0f, // color of vertex 10
                    0.0f, 1.0f, 0.0f, 1.0f, // color of vertex 11
                    // Colors to draw the Z axis
                        0.0f, 0.0f, 1.0f, 1.0f, // color of vertex 12
                        0.0f, 0.0f, 1.0f, 1.0f, // color of vertex 13
    };
    indexData = {
        // We define the indices of the vertices that define the FACES of a cube
        // through 12 triangles that will be drawn with the rendering mode
        // GL_TRIANGLES.
            0, 1, 2, // triangle 1 
            7, 6, 5, // triangle 2 
            3, 2, 6, // triangle 3 
            4, 0, 3, // triangle 4 
            4, 5, 1, // triangle 5 
            1, 5, 6, // triangle 6 
            0, 2, 3, // triangle 7 
            7, 5, 4, // triangle 8 
            3, 6, 7, // triangle 9 
            4, 3, 7, // triangle 10
            4, 1, 0, // triangle 11
            1, 6, 2, // triangle 12
            // We define the indices of the vertices that define the EDGES of a cube
            // through 12 lines that will be drawn with the rendering mode
            // GL_LINES.
                0, 1, // line 1 
                1, 2, // line 2 
                2, 3, // line 3 
                3, 0, // line 4 
                0, 4, // line 5 
                4, 7, // line 6 
                7, 6, // line 7 
                6, 2, // line 8 
                6, 5, // line 9 
                5, 4, // line 10
                5, 1, // line 11
                7, 3, // line 12
                // We define the indices of the vertices that define the lines of the X, Y,
                // Z axes, which will be drawn with the GL_LINES mode.
                    8 , 9 , // line 1
                    10, 11, // line 2
                    12, 13  // line 3
    };
    vertexSize = vertexData.size() * sizeof(GLfloat);
    colorSize = colorData.size() * sizeof(GLfloat);
    indexSize = indexData.size() * sizeof(GLuint);


    SceneObject cube_faces;
    cube_faces.name = "faces";
    cube_faces.first_index = (void*)0; // First index is at indexData[0]
    cube_faces.num_indices = 36;       // Last index is at indexData[35]; total of 36 indices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Indices correspond to the GL_TRIANGLES rasterization type.

    SceneObject cube_edges;
    cube_edges.name = "edges";
    cube_edges.first_index = (void*)(36 * sizeof(GLuint)); // First index is at indexData[36]
    cube_edges.num_indices = 24; // Last index is at indexData[59]; total of 24 indices.
    cube_edges.rendering_mode = GL_LINES; // Indices correspond to the GL_LINES rasterization type;
    cube_edges.has_color = false; // Edges have no associated color, will be drawn in black.

    SceneObject axes;
    axes.name = "axes";
    axes.first_index = (void*)(60 * sizeof(GLuint)); // First index is at indexData[60]
    axes.num_indices = 6; // Last index is at indexData[65]; total of 6 indices.
    axes.rendering_mode = GL_LINES; // Indices correspond to the GL_LINES rasterization type;
    axes.line_width = 10.0f; // Line width for the XYZ axes

    scene_objects.push_back(cube_faces);
    scene_objects.push_back(cube_edges);
    scene_objects.push_back(axes);
    setupMesh();
};
Cube::~Cube() {
    // Destroys the cube, releasing allocated resources
    scene_objects.clear();
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}
