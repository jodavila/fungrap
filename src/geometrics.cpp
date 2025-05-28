#include "../include/geometrics.hpp"

Cube::Cube() {
    // Definindo os dados dos vértices do cubo

    GLfloat vertex_data[] = {
         //    X      Y     Z     W
            -0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 0
            -0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 1
             0.5f, -0.5f,  0.5f, 1.0f, // posição do vértice 2
             0.5f,  0.5f,  0.5f, 1.0f, // posição do vértice 3
            -0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 4
            -0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 5
             0.5f, -0.5f, -0.5f, 1.0f, // posição do vértice 6
             0.5f,  0.5f, -0.5f, 1.0f, // posição do vértice 7
             // Vértices para desenhar o eixo X
             //    X      Y     Z     W
                  0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 8
                  1.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 9
                  // Vértices para desenhar o eixo Y
                  //    X      Y     Z     W
                       0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 10
                       0.0f,  1.0f,  0.0f, 1.0f, // posição do vértice 11
                       // Vértices para desenhar o eixo Z
                       //    X      Y     Z     W
                            0.0f,  0.0f,  0.0f, 1.0f, // posição do vértice 12
                            0.0f,  0.0f,  1.0f, 1.0f, // posição do vértice 13
    };
    GLfloat color_data[] = {
        // Cores dos vértices do cubo
        //  R     G     B     A
            1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 0
            1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 1
            0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 2
            0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 3
            1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 4
            1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 5
            0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 6
            0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 7
            // Cores para desenhar o eixo X
                1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 8
                1.0f, 0.0f, 0.0f, 1.0f, // cor do vértice 9
                // Cores para desenhar o eixo Y
                    0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 10
                    0.0f, 1.0f, 0.0f, 1.0f, // cor do vértice 11
                    // Cores para desenhar o eixo Z
                        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 12
                        0.0f, 0.0f, 1.0f, 1.0f, // cor do vértice 13
    };
    GLuint index_data[] = {
        // Definimos os índices dos vértices que definem as FACES de um cubo
        // através de 12 triângulos que serão desenhados com o modo de renderização
        // GL_TRIANGLES.
            0, 1, 2, // triângulo 1 
            7, 6, 5, // triângulo 2 
            3, 2, 6, // triângulo 3 
            4, 0, 3, // triângulo 4 
            4, 5, 1, // triângulo 5 
            1, 5, 6, // triângulo 6 
            0, 2, 3, // triângulo 7 
            7, 5, 4, // triângulo 8 
            3, 6, 7, // triângulo 9 
            4, 3, 7, // triângulo 10
            4, 1, 0, // triângulo 11
            1, 6, 2, // triângulo 12
            // Definimos os índices dos vértices que definem as ARESTAS de um cubo
            // através de 12 linhas que serão desenhadas com o modo de renderização
            // GL_LINES.
                0, 1, // linha 1 
                1, 2, // linha 2 
                2, 3, // linha 3 
                3, 0, // linha 4 
                0, 4, // linha 5 
                4, 7, // linha 6 
                7, 6, // linha 7 
                6, 2, // linha 8 
                6, 5, // linha 9 
                5, 4, // linha 10
                5, 1, // linha 11
                7, 3, // linha 12
                // Definimos os índices dos vértices que definem as linhas dos eixos X, Y,
                // Z, que serão desenhados com o modo GL_LINES.
                    8 , 9 , // linha 1
                    10, 11, // linha 2
                    12, 13  // linha 3
    };
    vertexData = (void*)vertex_data;
    colorData = (void*)color_data;
    indexData = (void*)index_data;

    SceneObject cube_faces;
    cube_faces.name = "faces";
    cube_faces.first_index = (void*)0; // Primeiro índice está em indexData[0]
    cube_faces.num_indices = 36;       // Último índice está em indexData[35]; total de 36 índices.
    cube_faces.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    SceneObject cube_edges;
    cube_edges.name = "edges";
    cube_edges.first_index = (void*)(36 * sizeof(GLuint)); // Primeiro índice está em indexData[36]
    cube_edges.num_indices = 24; // Último índice está em indexData[59]; total de 24 índices.
    cube_edges.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES;
    cube_edges.has_color = false; // Arestas não têm cor associada, serão desenhadas com a cor preta.

    SceneObject axes;
    axes.name = "axes";
    axes.first_index = (void*)(60 * sizeof(GLuint)); // Primeiro índice está em indexData[60]
    axes.num_indices = 6; // Último índice está em indexData[65]; total de 6 índices.
    axes.rendering_mode = GL_LINES; // Índices correspondem ao tipo de rasterização GL_LINES;
    axes.has_color = false; // Eixos não têm cor associada, serão desenhados com a cor preta.
    
    scene_objects = std::vector<SceneObject>();
    scene_objects.push_back(cube_faces);
    scene_objects.push_back(cube_edges);
    scene_objects.push_back(axes);
    setupMesh();
};
Cube::~Cube() {
    // Destrói o cubo, liberando os recursos alocados
    scene_objects.clear();
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}