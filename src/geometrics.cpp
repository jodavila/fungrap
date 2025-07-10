#include "../include/geometrics.hpp"
#include "../include/matrices.hpp"
#include "glm/gtx/string_cast.hpp"
#include "physics.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>

GLuint g_NumLoadedTextures = 0;

glm::vec4 Mesh::ComputeNormals(bool force) {
    if (force) {
        // Force computation of normals even if they are already present
        return ComputeNormals();
    }
    if (!model->attrib.normals.empty()) {
        return glm::vec4(model->attrib.normals[0],
            model->attrib.normals[1],
            model->attrib.normals[2],
            0.0f);
    }
    return ComputeNormals();
}

glm::vec4 Mesh::ComputeNormals() {
    // This function computes normals for the mesh if needed.
    // Implementation details would depend on the specific mesh structure.

    {

        size_t num_vertices = model->attrib.vertices.size() / 3;

        std::vector<int> num_triangles_per_vertex(num_vertices, 0);
        std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

        for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
            size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

            for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
                assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

                glm::vec4  vertices[3];
                for (size_t vertex = 0; vertex < 3; ++vertex) {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                    const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                    const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                    const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                    vertices[vertex] = glm::vec4(vx, vy, vz, 1.0);
                }

                const glm::vec4  a = vertices[0];
                const glm::vec4  b = vertices[1];
                const glm::vec4  c = vertices[2];

                const glm::vec4  n = normalize(crossproduct((b - a), (c - a)));

                for (size_t vertex = 0; vertex < 3; ++vertex) {
                    tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                    num_triangles_per_vertex[idx.vertex_index] += 1;
                    vertex_normals[idx.vertex_index] += n;
                    model->shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = idx.vertex_index;
                }
            }
        }

        model->attrib.normals.resize(3 * num_vertices);

        for (size_t i = 0; i < vertex_normals.size(); ++i) {
            glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
            n /= norm(n);
            model->attrib.normals[3 * i + 0] = n.x;
            model->attrib.normals[3 * i + 1] = n.y;
            model->attrib.normals[3 * i + 2] = n.z;
        }
    }
    if (model->attrib.normals.empty()) {
        fprintf(stderr, "Error: No normals found in the model. Normals will not be computed.\n");
        return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    if (model->attrib.vertices.size() < 9) {
        fprintf(stderr, "Error: Model has less than 3 . Normals cannot be computed.\n");
        return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec4 p0(model->attrib.vertices[0], model->attrib.vertices[1], model->attrib.vertices[2], 1.0f);
    glm::vec4 p1(model->attrib.vertices[3], model->attrib.vertices[4], model->attrib.vertices[5], 1.0f);
    glm::vec4 p2(model->attrib.vertices[6], model->attrib.vertices[7], model->attrib.vertices[8], 1.0f);
    glm::vec4 u = p1 - p0;
    glm::vec4 v = p2 - p0;
    glm::vec4 n = crossproduct(u, v) / norm(crossproduct(u, v));

    return n;

}


void Mesh::BuildTrianglesAndAddToVirtualScene(VirtualScene& scene) {
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;   // posição (vec4)
    std::vector<float>  normal_coefficients;  // normais (vec4)
    std::vector<float>  texture_coefficients; // textura (vec2)

    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                indices.push_back(first_index + 3 * triangle + vertex);

                // posição (vec4)
                const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                model_coefficients.push_back(vx);
                model_coefficients.push_back(vy);
                model_coefficients.push_back(vz);
                model_coefficients.push_back(1.0f);

                // normais (vec4)
                if (idx.normal_index != -1) {
                    const float nx = model->attrib.normals[3 * idx.normal_index + 0];
                    const float ny = model->attrib.normals[3 * idx.normal_index + 1];
                    const float nz = model->attrib.normals[3 * idx.normal_index + 2];
                    normal_coefficients.push_back(nx);
                    normal_coefficients.push_back(ny);
                    normal_coefficients.push_back(nz);
                    normal_coefficients.push_back(0.0f); // normal homogênea
                }

                // textura (vec2)
                if (idx.texcoord_index != -1) {
                    const float u = model->attrib.texcoords[2 * idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2 * idx.texcoord_index + 1];
                    texture_coefficients.push_back(u);
                    texture_coefficients.push_back(v);
                }
            }
        }

        size_t last_index = indices.size() - 1;

        SceneObject obj;
        obj.name = model->shapes[shape].name;
        obj.first_index = first_index;
        obj.num_indices = last_index - first_index + 1;
        obj.rendering_mode = GL_TRIANGLES;
        obj.vao = vertex_array_object_id;
        obj.has_color = this->has_color;
        scene.scene_objects[obj.name] = obj;
        this->name = obj.name; // atualiza nome do Mesh
    }

    // Posição
    GLuint VBO_pos;
    glGenBuffers(1, &VBO_pos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), model_coefficients.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Normais
    if (!normal_coefficients.empty()) {
        GLuint VBO_norm;
        glGenBuffers(1, &VBO_norm);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_norm);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), normal_coefficients.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }

    // Coordenadas de textura
    if (!texture_coefficients.empty()) {
        GLuint VBO_tex;
        glGenBuffers(1, &VBO_tex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_tex);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), texture_coefficients.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
    }

    // Índices
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0); // desliga VAO
}

Mesh::Mesh(std::string filename) {
    model = new ObjModel(filename.c_str());
    puts("Mesh::Mesh(): Model loaded successfully.");
    ComputeNormals();
    transform = Matrix_Identity();
    this->body = new RigidBody();
}
Mesh::~Mesh() {
    delete model;
    delete body;
    puts("Mesh::~Mesh(): Model and body deleted successfully.");
}

glm::vec3 Mesh::compute_bbox_min(const tinyobj::attrib_t& attrib) {
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();

    for (size_t v = 0; v < attrib.vertices.size() / 3; ++v) {
        float x = attrib.vertices[3 * v + 0];
        float y = attrib.vertices[3 * v + 1];
        float z = attrib.vertices[3 * v + 2];

        min_x = std::min(min_x, x);
        min_y = std::min(min_y, y);
        min_z = std::min(min_z, z);
    }

    return glm::vec3(min_x, min_y, min_z);
}

glm::vec3 Mesh::compute_bbox_max(const tinyobj::attrib_t& attrib) {
    float max_x = -std::numeric_limits<float>::max();
    float max_y = -std::numeric_limits<float>::max();
    float max_z = -std::numeric_limits<float>::max();

    for (size_t v = 0; v < attrib.vertices.size() / 3; ++v) {
        float x = attrib.vertices[3 * v + 0];
        float y = attrib.vertices[3 * v + 1];
        float z = attrib.vertices[3 * v + 2];

        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);
        max_z = std::max(max_z, z);
    }

    return glm::vec3(max_x, max_y, max_z);
}



void Mesh::updateTransform() {
    glm::vec4 translate = body->getPosition();
    glm::vec3 rotate = body->getRotation();
    glm::vec4 scale = body->getScale();
    glm::vec3 pivot = body->getPivot();

    glm::mat4 T = Matrix_Translate(translate.x, translate.y, translate.z);
    glm::mat4 S = Matrix_Scale(scale.x, scale.y, scale.z);
    glm::mat4 Rx = Matrix_Rotate_X(rotate.x);
    glm::mat4 Ry = Matrix_Rotate_Y(rotate.y);
    glm::mat4 Rz = Matrix_Rotate_Z(rotate.z);
    glm::mat4 R = Rz * Ry * Rx;

    glm::mat4 P = Matrix_Translate(+pivot.x, +pivot.y, +pivot.z);
    glm::mat4 Pi = Matrix_Translate(-pivot.x, -pivot.y, -pivot.z);

    transform = T * P * R * S * Pi;
}

void Mesh::sendTransform(GLint program_id) {
    // Envia a matriz de transformação para o shader
    glUniformMatrix4fv(program_id, 1, GL_FALSE, glm::value_ptr(transform));
}

// Em geometrics.cpp:

void Mesh::rescale(float sx, float sy, float sz) {
    // 1) Escala **de verdade** os atributos de vértice:
    auto& verts = this->model->attrib.vertices;  // std::vector<float>
    for (size_t i = 0; i < verts.size(); i += 3) {
        verts[i + 0] *= sx;
        verts[i + 1] *= sy;
        verts[i + 2] *= sz;
    }

    this->ComputeNormals();

    glm::vec4 center = body->ComputeRigidBodyCenter(this->model);
    body->setPivot(glm::vec4(center));

}

void Mesh::LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
    this->setTexture(texture_id); // Set the texture ID in the Mesh class
}


Cube::Cube(float size, std::string model_filename) {
    this->size = size;
    width = height = depth = size; // Set width, height, and depth to size
    model = new ObjModel(model_filename.c_str());
    body = new RigidBody();
    rescale(size, size, size); // Rescale to size
    setPivot(getMeshCenter());
    transform = Matrix_Identity();
    ComputeNormals();
}
Cube::Cube(float size, std::string model_filename, glm::vec4 position) : Cube(size, model_filename) {
    body->setPosition(position);
}
Cube::Cube(float width, float height, float depth, std::string model_filename, glm::vec4 position) {
    model = new ObjModel(model_filename.c_str());
    body = new RigidBody();
    rescale(width, height, depth); // Rescale to width, height, and depth
    setPivot(getMeshCenter());
    transform = Matrix_Identity();
    ComputeNormals();
    for (const auto& v : model->attrib.vertices) {
        // Print the vertex coordinates
        std::cout << "Vertex: " << v << std::endl;
    }


    body->setPosition(position);
}

Plane::Plane(float width, float height, std::string model_filename) {
    this->width = width;
    this->height = height;
    model = new ObjModel(model_filename.c_str());
    body = new RigidBody();
    transform = Matrix_Identity();
    rescale(width, height, 1.0f); // Rescale to width and height
    //body->setScale(glm::vec4(width, 1.0f, height, 1.0f)); // Set scale to width and height

    setPivot(getMeshCenter());
    normal = ComputeNormals(true);
}
Plane::Plane(float width, float height, glm::vec4 position, std::string model_filename) : Plane(width, height, model_filename) {
    body->setPosition(position);
}

Ball::Ball(float radius, std::string model_filename) {

    model = new ObjModel(model_filename.c_str());
    body = new RigidBody();
    transform = Matrix_Identity();
    rescale(radius, radius, radius); // Rescale to the radius
    //body->setScale(glm::vec4(radius, radius, radius, 1.0f)); // Set scale to radius
    setPivot(getMeshCenter());
    this->radius = radius; // Set the radius of the ball
    ComputeNormals();
}
Ball::Ball(float radius, glm::vec4 position, std::string model_filename) : Ball(radius, model_filename) {
    body->setPosition(position);
}

Cylinder::Cylinder(float radius, float height, std::string model_filename) {
    this->radius = radius;
    this->height = height;
    model = new ObjModel(model_filename.c_str());
    body = new RigidBody();
    setPivot(getMeshCenter());
    rescale(radius, height, radius); // Rescale to radius and height
    transform = Matrix_Identity();
    ComputeNormals();
}
Cylinder::Cylinder(float radius, float height, glm::vec4 position, std::string model_filename) : Cylinder(radius, height, model_filename) {
    body->setPosition(position);
}

void Ball::testCollisionWithPlane(Plane* plane) {
    // Test collision with a plane
    collisor col;
    if (col.SphereToPlane(*this, *plane)) {
        glm::vec4 normal = plane->normal; // Get the normal vector of the plane
        glm::vec4 velocity = body->getVelocity(); // Get the velocity of the ball
        glm::vec4 v_normal = glm::dot(velocity, normal) * normal;
        glm::vec4 v_tangential = velocity - v_normal; // Calculate the tangential velocity
        body->setVelocity(-v_normal + v_tangential); // Set the velocity of the ball to the tangential velocity

        glm::vec4 normal_force = -glm::dot(g, normal) * normal * body->getMass();

        body->addForce(normal_force);
        isGrounded = true;

    }
    
}

void Ball::testCollisionWithCube(Cube* cube) {
    collisor col;

    if (col.SphereToCube(*this, *cube)) {
        //std::cout << "Collision with cube detected!" << std::endl;
        body->setPosition(glm::vec4 (0.0f,3.0f,0.0f,1.0f));
        body->resetAcceleration();
        body->resetVelocity();

    }


    
}
void Ball::testCollisionWithCylinder(Cylinder* cylinder) {
    collisor col;
    //If ball in the radius of the cylinder, turn off the normal force
    if(col.SphereToCylinder(*this, *cylinder)) {
        this->body->resetForce();
        this->body->addForce(g); //Only gravity force
        //std::cout << "Collision with radius of cylinder detected!" << std::endl;
    }
    if(col.SphereToCylinderBottom(*this, *cylinder)) {
        //If ball hit the bottom of the cylinder, turn on normal force
      //  std::cout << "Collision with bottom of cylinder detected!" << std::endl;
        glm::vec4 normal = glm::normalize(getCenter() - cylinder->getCenter());
        glm::vec4 velocity = body->getVelocity(); // Get the velocity of the ball
        glm::vec4 v_normal = glm::dot(velocity, normal) * normal;
        glm::vec4 v_tangential = velocity - v_normal; // Calculate the tangential velocity
        body->setVelocity(-v_normal + v_tangential); // Set the velocity of
        // the ball to the tangential velocity
        glm::vec4 normal_force = -glm::dot(g, normal) * normal * body->getMass();
        body->addForce(normal_force);
    }

}

GolfClub::GolfClub(float length, float width, float height, std::string model_filename) {
    this->length = length;
    this->width = width;
    this->height = height;
    model = new ObjModel(model_filename.c_str());
    body = new RigidBody();
    transform = Matrix_Identity();
    ComputeNormals();
    rescale(length, width, height); // Rescale the golf club to the specified dimensions
    setPivot(getMeshCenter());
    //body->setScale(glm::vec4(length, width, height, 1.0f)); // Set scale to length, width, and height
    glm::vec3 club_size = getMeshSize(model); // Get the size of the golf club mesh
    collision_box = new Cube(length, width, height, "../../assets/objects/unit_cube.obj", this->body->getPosition());
    collision_box->rescale(club_size.x, club_size.y, club_size.z);
    collision_box->body = this->body; // Set the collision box's body to the golf club's body
    collision_box->setPivot(getMeshCenter());
    collision_box->transform = Matrix_Identity();
}

GolfClub::GolfClub(float length, float width, float height, glm::vec4 position, std::string model_filename) : GolfClub(length, width, height, model_filename) {
    body->setPosition(position);
}
void GolfClub::animate(float deltaTime, GLFWwindow* window) {
    // Pressione espaço para iniciar o swing
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && swing_state == Idle) {
        swing_state = Backswing;
        swing_timer = 0.0f;
    }

    if (swing_state == Backswing) {
        swing_timer += deltaTime;
        float t = swing_timer / (swing_duration * 0.5f); // metade do tempo para o backswing
        if (t >= 1.0f) {
            t = 1.0f;
            swing_state = Downswing;
            swing_timer = 0.0f;
        }

        // Inverte direção do backswing
        float angle = glm::mix(impact_angle, backswing_angle, t);
        body->setRotation(glm::vec4(angle, body->getRotation().y, body->getRotation().z, 1.0f));
    }
    else if (swing_state == Downswing) {
        swing_timer += deltaTime;
        float t = swing_timer / (swing_duration * 0.25f); // mais rápido na volta
        if (t >= 1.0f) {
            t = 1.0f;
            swing_state = Idle;
        }

        // Inverte direção da descida
        float angle = glm::mix(backswing_angle, impact_angle, t);
        body->setRotation(glm::vec4(angle, body->getRotation().y, body->getRotation().z, 1.0f));
    }
}

void GolfClub::move(GLFWwindow* window) {
    float dx = 0.0f;
    float dz = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dz = -1.0f; // Move para frente


    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dz = 1.0f; // Move para trás

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dx = -1.0f; // Move para a esquerda

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dx = 1.0f; // Move para a direita

    glm::vec4 direction = glm::vec4(dx, 0.0f, dz, 0.0f);
    if (glm::length(direction) < 0.01f) {
        return; // Não há movimento
    }
    float speed = 10.0f;
    direction = glm::normalize(direction); // Normaliza a direção
    body->setVelocity(direction * speed); // Define a velocidade do golf club
}

void BezierCurve::setTrajectoryFromVelocity(glm::vec4 p0, glm::vec4 v0, glm::vec4 a, float bounce_factor)
{
    float g = -a.y; // aceleração da gravidade (valor positivo)
    if (g <= 0.0f) return;

    // Tempo para atingir o ponto mais alto
    float t1 = v0.y / g;
    // Ponto mais alto
    glm::vec4 p1 = p0 + v0 * t1 + 0.5f * a * (t1 * t1);

    float delta = (v0.y * v0.y) - (2.0f * g * p0.y);
    if (delta < 0.0f) delta = 0.0f;
    float t2 = (v0.y + sqrt(delta)) / g;

    // Ponto final 
    glm::vec4 p3 = p0 + v0 * t2 + 0.5f * a * (t2 * t2);

    glm::vec4 p2 = p1;

    // Define os pontos de controle da curva de Bézier
    setPoints({ p0, p1, p2, p3 });
    computeInterpolatedPoints(0.01f);
    config();
}

