#ifndef _GEOMETRICS_HPP
#define _GEOMETRICS_HPP
#include "glcontext.hpp"
#include "physics.hpp"
#include "tiny_obj_loader.h"
#include "collisions.hpp"
// We define a structure that will store the necessary data to render
// each object in the virtual scene.
//
// Structure representing a geometric model loaded from a ".obj" file.
// See https://en.wikipedia.org/wiki/Wavefront_.obj_file .
//
extern GLuint g_NumLoadedTextures;


struct ObjModel {
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // This constructor reads the model from a file using the tinyobjloader library.
    // See: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true) {
        printf("Loading objects from file \"%s\"...\n", filename);

        // If basepath == NULL, set basepath as the dirname of filename,
        // so that MTL files are correctly loaded if they are in the same directory as the OBJ files.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL) {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos) {
                dirname = fullpath.substr(0, i + 1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Error loading model.");

        for (size_t shape = 0; shape < shapes.size(); ++shape) {
            if (shapes[shape].name.empty()) {
                fprintf(stderr,
                    "*********************************************\n"
                    "Error: Object without a name inside file '%s'.\n"
                    "See https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                    "*********************************************\n",
                    filename);
                throw std::runtime_error("Object without a name.");
            }
            printf("- Object '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }



};

class BezierCurve {
private:
    GLuint vao, vbo;
    std::vector<glm::vec4> interpolated_points; // Coefficients of the Bezier curve
    std::vector<glm::vec4> control_points = {
        {0.0f, 3.0f, 0.0f, 1.0f}, // P0
        {1.0f, 3.0f, 0.0f, 1.0f}, // P1
        {2.0f, 3.0f, 0.0f, 1.0f}, // P2
        {3.0f, 3.0f, 0.0f, 1.0f}  // P3
    };
public:
    glm::mat4 transform;
    BezierCurve() {
        setPoints(control_points);
        computeInterpolatedPoints(0.01f); // Default step for interpolation
        config();
    }
    BezierCurve(std::vector<glm::vec4> control_points) {
        setPoints(control_points);
        computeInterpolatedPoints(0.01f); // Default step for interpolation
        config();
    }
    BezierCurve(std::vector<glm::vec4> control_points, float step) {
        setPoints(control_points);
        computeInterpolatedPoints(step);
        config();
    }
    ~BezierCurve() = default;

    inline void printInterpolatedPoints() {
        for (const auto& point : interpolated_points) {
            printf("Interpolated Point: (%f, %f, %f, %f)\n", point.x, point.y, point.z, point.w);
        }
    }
    
    inline void setPoints(std::vector<glm::vec4> points){
        control_points.clear();
        if (points.size() < 4) {
            fprintf(stderr, "Error: Bezier cubic Curve requires at least 4 control points.\n");
            return;
        }
        for (const auto& point : points) 
            control_points.push_back(point);
    }
    inline void computeInterpolatedPoints(float step){
        interpolated_points.clear();
        glm::vec4 B, P0 = control_points[0], P1 = control_points[1], P2 = control_points[2], P3 = control_points[3];
        for(float t = 0; t <= 1; t += step) {
            B = static_cast<float>(pow((1-t),3))*P0 + 3*static_cast<float>(pow((1-t),2))*t*P1 + 3*static_cast<float>((1-t)*t*t)*P2 + static_cast<float>(t*t*t)*P3;
            interpolated_points.push_back(B);
        }
        
    }

    inline void config() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, interpolated_points.size() * sizeof(glm::vec4), interpolated_points.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

    }

    inline void draw(GLuint program_id) {
        
        glUniform1i(glGetUniformLocation(program_id, "render_as_black"), true);
        glBindVertexArray(vao);
        glLineWidth(10.0f); // Define linha com 3 pixels de espessura
        glDrawArrays(GL_LINE_STRIP, 0, interpolated_points.size()); 
        glBindVertexArray(0);
        
    }
    void setTrajectoryFromVelocity(glm::vec4 p0, glm::vec4 v0, glm::vec4 a, float bounce_factor);

};



class Mesh {
protected:
    Mesh() = default;
    ObjModel* model = nullptr; // Pointer to the model loaded from a file
    std::string name;
    bool has_color = true; // If true, render the mesh as black
    int id;
    glm::vec4 ComputeNormals();
    glm::vec4 ComputeNormals(bool force);
    void BuildTrianglesAndAddToVirtualScene(VirtualScene& scene);
    GLuint texture_id = 0; // Texture ID for the mesh
    
    public:
    bool use_texture = false;
    class RigidBody* body; // Rigid body data
    glm::mat4 transform;
    Mesh(std::string filename);
    virtual ~Mesh();
    void rescale(float sx, float sy, float sz);
    inline void addToVirtualScene(VirtualScene& scene) {
        id = scene.getNextId();
        BuildTrianglesAndAddToVirtualScene(scene);
    }
    inline ObjModel* getModel() { return model; }
    inline std::string getName() { return name; }
    inline glm::mat4 getTransform() const { return transform; }
    inline void setColor(bool color) { has_color = color; }
    void updateTransform();
    inline void setTransform(glm::mat4 transform) { this->transform = transform; }
    void sendTransform(GLint program_id);
    inline glm::vec4 getMeshCenter() { return body->ComputeRigidBodyCenter(this->model); }
    inline glm::vec4 getCenter() { return getMeshCenter() + (body->getPosition() - getMeshCenter()); }
    inline void setPivot(const glm::vec4& p) { this->body->setPivot(p); }
    inline glm::mat4 getTransform() { return transform; }
    inline void setName(const std::string& n) { name = n; }
    glm::vec3 compute_bbox_min(const tinyobj::attrib_t& attrib);
    glm::vec3 compute_bbox_max(const tinyobj::attrib_t& attrib);
    inline glm::vec3 getMeshSize(ObjModel* model) {
        glm::vec3 min = compute_bbox_min(model->attrib);
        glm::vec3 max = compute_bbox_max(model->attrib);
        return max - min;
    }
    inline void setID(int newid) { id = newid; }
    void LoadTextureImage(const char* filename);
    inline void setTexture(GLuint texid) {
        use_texture = true;
        texture_id = texid;
    }

    inline void disableTexture() {
        use_texture = false;
    }

    inline bool isTextured() const {
        return use_texture;
    }

    inline GLuint getTextureId() const {
        return texture_id;
    }

};

class Cube : public Mesh {
public:
    float size; //distance from the center to the corner of the cube
    float width; // Width of the cube
    float height; // Height of the cube
    float depth; // Depth of the cube
    glm::vec4 normal;
    Cube() = default;
    Cube(float size, std::string model_filename);
    Cube(float size, std::string model_filename, glm::vec4 position);
    Cube(float width, float height, float depth, std::string model_filename, glm::vec4 position);
};

class Plane : public Mesh {
public:
    float width;  // Width of the plane
    float height; // Height of the plane
    glm::vec4 normal; // Normal vector of the plane
    Plane() = default; // Default constructor is deleted to prevent instantiation without parameters
    Plane(float width, float height, std::string model_filename);
    Plane(float width, float height, glm::vec4 position, std::string model_filename);
};


class Ball : public Mesh {
private:
    // The ball is a sphere with a radius, position, rotation, and scale.
    // It uses the ObjModel class to load the model from a file.
    // The RigidBody class is used to handle the physics of the ball.
    // The transform matrix is used to apply transformations to the ball.
    Ball() = default; // Default constructor is deleted to prevent instantiation without parameters
public:
    bool isGrounded = true;
    float radius; // Radius of the ball
    Ball(float radius, std::string model_filename);
    Ball(float radius, glm::vec4 position, std::string model_filename);
    void testCollisionWithPlane(Plane* plane);
    void testCollisionWithCube(Cube* cube);
    void testCollisionWithCylinder(Cylinder* cylinder);
};
class Cylinder : public Mesh {
public:
    float radius; // Radius of the cylinder
    float height; // Height of the cylinder
    Cylinder() = default; // Default constructor is deleted to prevent instantiation without parameters
    Cylinder(float radius, float height, std::string model_filename);
    Cylinder(float radius, float height, glm::vec4 position, std::string model_filename);

};

class GolfClub : public Mesh {
private:
    float length; // Length of the golf club
    float width;  // Width of the golf club 
    float height; // Height of the golf club
    enum SwingState {
        Backswing,
        Downswing,
        Idle
    };
    SwingState swing_state = Idle; // Current state of the swing
    float swing_duration = 1.0f; // Duration of the swing in seconds
    float swing_timer = 0.0f; // Timer for the swing animation
    float backswing_angle = -M_PI / 6.0f; // 67.5 degrees
    float impact_angle = -M_PI / 2.0f; // taco reto para frente

public:
    bool controller = true;
    Cube* collision_box; // Collision box for the golf club
    GolfClub() = default; // Default constructor is deleted to prevent instantiation without parameters
    GolfClub(float length, float width, float height, std::string model_filename);
    GolfClub(float length, float width, float height, glm::vec4 position, std::string model_filename);
    inline void updateCollisionBox() {
        *collision_box->body = *this->body; // Update the collision box's body to match the golf club's body
    }
    void animate(float deltaTime, GLFWwindow* window); // Animate the golf club
    void move(GLFWwindow* window); // Move the golf club based on user input
};
#endif // _GEOMETRICS_HPPs