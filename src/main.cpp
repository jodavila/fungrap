#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers below are specific to C++
#include <map>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtx/string_cast.hpp>

// Local headers, defined in the "include/" folder
#include "../include/utils.h"
#include "../include/GLFW/glfw3.h"  // Window creation for the operating system
#include "../include/matrices.hpp"
#include "../include/camera.hpp"
#include "../include/timer.hpp"
#include "../include/geometrics.hpp"
#include "../include/glcontext.hpp"
#include "../include/tiny_obj_loader.h"
#include "../include/collisions.hpp"

// Identificador que define qual objeto está sendo desenhado no momento
#define BALL   0
#define CLUB  1
#define WALL  2
#define FLOOR 3
#define VEGETATION 4
#define CLOUDS 5

// Declaration of several functions used in main(). These are defined
// right after the definition of main() in this file.
GLuint BuildTriangles(); // Builds triangles for rendering
void LoadShadersFromFiles(); // Loads vertex and fragment shaders, creating a GPU program
GLuint LoadShader_Vertex(const char* filename);   // Loads a vertex shader
GLuint LoadShader_Fragment(const char* filename); // Loads a fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Function used by the two above
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Creates a GPU program

// Declaration of helper functions to render text inside the OpenGL window.
// These functions are defined in the file "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string& str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

// Functions below render as text in the OpenGL window some matrices and
// other program information. Defined after main().
void TextRendering_ShowModelViewProjection(GLFWwindow* window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model);
void TextRendering_ShowEulerAngles(GLFWwindow* window);
void TextRendering_ShowProjection(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Callback functions for communication with the operating system and user interaction.

// See more comments in their definitions below.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


// Below we define global variables used in various functions of the code.

// The virtual scene is a list of named objects, stored in a dictionary
// (map). See inside the BuildTriangles() function how objects are included
// in the g_VirtualScene variable, and see in the main() function how
// these are accessed.
std::map<const char*, SceneObject> g_VirtualScene;

// Window aspect ratio (width/height). See FramebufferSizeCallback() function.
float g_ScreenRatio = 1.0f;

// Euler angles that control the rotation of one of the cubes in the virtual scene
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;
float axis = 1.0f;

// "g_LeftMouseButtonPressed = true" if the user is currently pressing the left mouse button
// See MouseButtonCallback() function.


// Variables that define the camera in spherical coordinates, controlled by
// the user through the mouse (see CursorPosCallback() function). The effective
// camera position is calculated inside the main() function, within the rendering loop.

bool isFreeCamera = true;
bool reset = false; // Variable to reset the camera position
glm::vec4 position = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f); // free camera position
glm::vec4 camera_view_vector;
glm::vec4 camera_up_vector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // "up" vector fixed to point to the "sky" (global Y axis)
float speed = 1.0f;
double deltaTime = 0.0f;


// Variable that controls the type of projection used: perspective or orthographic.
bool g_UsePerspectiveProjection = true;

// Variable that controls whether the informational text will be shown on the screen.
bool g_ShowInfoText = true;

// Variables that define a GPU program (shaders). See LoadShadersFromFiles() function.
GLuint g_GpuProgramID = 0;


bool aimEnabled = false; // Set to true to enable aiming at the ball
float strength = 10.0f; // Strength of the force applied to the ball
struct TEST {
    TEST() = default;
    float x = 0.0f;
    float y = 0.05f; // Slightly above the ground
    float z = 0.8f; // Initial position of the ball
    int dx = 0; // Change in x position
    int dy = 0; // Change in y position
    int dz = 0; // Change in z position
    bool teleport = false; // Teleport flag to move the ball to a new position
    bool reset = false; // Reset flag to reset the ball position
    bool hit = false; // Flag to indicate if the ball has been hit
    bool aim = false;
    bool debug = false; // Debug flag to print debug information
    bool manualControl = false; // Flag to enable manual control of the ball
    inline glm::vec4 getDirection() {
        return glm::vec4(this->dx, this->dy, this->dz, 0.0f);
    }
    inline void resetDirection() {
        this->dx = 0;
        this->dy = 0;
        this->dz = 0;
    }
};
struct TEST test;

int main() {
    // We initialize the GLFW library, used to create a window of the
    // operating system, where we can render with OpenGL.
    int success = glfwInit();
    if (!success) {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // We set the callback for printing GLFW errors to the terminal
    glfwSetErrorCallback(ErrorCallback);

    // We request to use OpenGL version 3.3 (or higher)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // We request to use the "core" profile, that is, we will use only the
    // modern OpenGL functions.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // We create an operating system window, with 800 columns and 800 rows
    // of pixels, and with the title "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 800, "Mini-Golf 3D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // We set the callback function that will be called whenever the user
    // presses a keyboard key ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... or clicks the mouse buttons ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... or moves the mouse cursor over the window ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... or scrolls the mouse "wheel".
    glfwSetScrollCallback(window, ScrollCallback);

    // We set the callback function that will be called whenever the window is
    // resized, consequently changing the size of the "framebuffer"
    // (memory region where the image pixels are stored).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowSize(window, 800, 800); // We force the callback above to be called, to set g_ScreenRatio.

    // We indicate that OpenGL calls should render in this window
    glfwMakeContextCurrent(window);

    // Loading of all functions defined by OpenGL 3.3, using the
    // GLAD library.
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // We print to the terminal information about the system's GPU
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glversion = glGetString(GL_VERSION);
    const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // We load the vertex and fragment shaders that will be used
    // for rendering. See slides 180-200 of the document Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();


    // We initialize the code for text rendering.
    TextRendering_Init();

    // We get the address of the variables defined inside the Vertex Shader.
    // We will use these variables to send data to the video card
    // (GPU)! See file "shader_vertex.glsl".
    GLint model_uniform = glGetUniformLocation(g_GpuProgramID, "model"); // "model" matrix variable
    GLint view_uniform = glGetUniformLocation(g_GpuProgramID, "view"); // "view" matrix variable in shader_vertex.glsl
    GLint projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // "projection" matrix variable in shader_vertex.glsl
    // We enable the Z-buffer. See slides 104-116 of the document Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Auxiliary variables used for calling the
    // TextRendering_ShowModelViewProjection() function, storing 4x4 matrices.
    glm::mat4 the_projection;
    // glm::mat4 the_model;
    glm::mat4 the_view;
    float half_pi = M_PI / 2.0f;
    // We stay in an infinite loop, rendering, until the user closes the window
    double lastFrame = glfwGetTime();
    Freecam* freecam = new Freecam();
    freecam->setPosition(glm::vec4(3.0f, 1.0f, 0.0f, 1.0f)); // Set the camera position
    Lookatcam* lookatcam = new Lookatcam();
    lookatcam->setPosition(glm::vec4(5.0f, 2.0f, 0.0f, 1.0f)); // Set the camera position
    VirtualScene* virtual_scene = new VirtualScene();
    std::vector<Mesh*> meshes;

    float wall_height = 2.0f; // Altura da parede
    float wall_width = 10.0f; // Largura da parede
    float floor_length = 5.0f; // Comprimento do piso  
    float floor_width = 5.0f; // Largura do piso
    float roof_height = wall_height * 4 * 2;

    Ball* ball = new Ball(0.02f, "../../assets/objects/golf_ball.obj");
    Plane* floor = new Plane(floor_width, floor_length, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), "../../assets/objects/floor.obj");
    Plane* roof = new Plane(floor_width, floor_length, glm::vec4(0.0f, roof_height, 0.0f, 1.0f), "../../assets/objects/roof.obj");
    Cube* cloud = new Cube(15.0f, "../../assets/objects/cloud.obj", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    Cube *void_zone = new Cube(floor_width*100, 10.0f, floor_length*100,  "../../assets/objects/unit_cube.obj", glm::vec4(0.0f, -30.0f, 0.0f, 1.0f));

    floor->body->setPosition(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    ball->body->setPosition(glm::vec4(0.1f, 2.0f, 0.8f, 1.0f));
    
   

    floor->body->setRotation(glm::vec4(-half_pi, 0.0f, 0.0f, 1.0f));
    roof->body->setRotation(glm::vec4(-half_pi, 0.0f, 0.0f, 1.0f));



    Plane* wall_north = new Plane(wall_width, wall_height, "../../assets/objects/wall_north.obj");
    Plane* wall_south = new Plane(wall_width, wall_height, "../../assets/objects/wall_south.obj");
    Plane* wall_east = new Plane(wall_width, wall_height, "../../assets/objects/wall_east.obj");
    Plane* wall_west = new Plane(wall_width, wall_height, "../../assets/objects/wall_west.obj");
    wall_north->body->setPosition(glm::vec4(0.0f, -wall_height / 4, -wall_width * 4, 1.0f));
    wall_south->body->setPosition(glm::vec4(0.0f, -wall_height / 4, wall_width * 4, 1.0f));
    wall_east->body->setPosition(glm::vec4(wall_width * 4, -wall_height / 4, 0.0f, 1.0f));
    wall_west->body->setPosition(glm::vec4(-wall_width * 4, -wall_height / 4, 0.0f, 1.0f));

    wall_north->normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    wall_south->normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    wall_east->normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    wall_west->normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    floor->normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Normal vector for the floor
    roof->normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f); // Normal vector for the roof

    wall_east->body->setRotation(glm::vec4(0.0f, half_pi, 0.0f, 0.0f)); // gira X depois Y +90°
    wall_west->body->setRotation(glm::vec4(0.0f, -half_pi, 0.0f, 0.0f)); // gira X depois Y -90°

    std::vector<Plane*> walls;
    walls.push_back(wall_north);
    walls.push_back(wall_south);
    walls.push_back(wall_east);
    walls.push_back(wall_west);
    walls.push_back(roof);
    walls.push_back(floor);

    for (auto& wall : walls) {
        wall->setID(WALL); // Set the ID of the wall
        wall->addToVirtualScene(*virtual_scene); // Add the walls to the virtual scene
        if(wall->getName() != "floor")
        wall->LoadTextureImage("../../assets/textures/sky.jpg"); // Load the wall texture
        meshes.push_back(wall);
    }
    floor->LoadTextureImage("../../assets/textures/forrest_ground_01_diff_1k.jpg");
    ball->LoadTextureImage("../../assets/textures/blue_metal_plate_diff_2k.jpg");
    cloud->LoadTextureImage("../../assets/textures/aerial_beach_01_diff_1k.jpg");
    

    ball->setID(BALL); // Set the ID of the ball
    //golf_club->setID(CLUB); // Set the ID of the golf club
    floor->setID(VEGETATION); // Set the ID of the floor
    cloud->setID(CLOUDS); // Set the ID of the cloud


    int count_clouds = 10; // Number of clouds to create
    std::vector<glm::mat4> cloud_transforms;
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
    float max_x = floor_width * 10; // Maximum X position for the clouds
    float max_y = roof_height; // Maximum Y position for the clouds
    float max_z = floor_length * 10; // Maximum Z position for the clouds
    float padding = 20.0f; // Padding to avoid clouds overlapping with walls
    for (int i = 0; i < count_clouds; ++i) {

        float x = static_cast<float>((rand() % (int)((max_x - padding) * 2)) - (max_x - padding));
        float y = max_y - padding / 4;
        float z = static_cast<float>((rand() % (int)((max_z - padding) * 2)) - (max_z - padding));
        cloud->body->setPosition(glm::vec4(x, y, z, 1.0f));
        cloud->updateTransform(); // Set the transform of the cloud
        cloud_transforms.push_back(cloud->getTransform()); // Store the transform of the cloud
    }

    float radius = 0.6f; // Radius of the hole
    float height = 1.5f; // Height of the hole
    Cylinder* hole = new Cylinder(radius, height, "../../assets/objects/hole.obj");
    hole->body->setPosition(glm::vec4(10.0f, -height + 0.01f, 0.0f, 1.0f)); // Set the position of the hole
    meshes.push_back(floor);
    meshes.push_back(ball);
    meshes.push_back(hole);

    ball->addToVirtualScene(*virtual_scene);
    cloud->addToVirtualScene(*virtual_scene);
    hole->addToVirtualScene(*virtual_scene);

    ball->body->setMass(0.2f); // Set the mass of the ball


    BezierCurve* bezier_curve = new BezierCurve();

    std::cout << "Running the Mini-Golf 3D simulation...\n";
    camera_distance = lookatcam->camera_distance;
    float r = camera_distance;
    glm::vec4 ball_position = ball->getCenter();
    glm::vec4 camera_position = ball_position + glm::vec4(r,r,r,0.0f);
    while (!glfwWindowShouldClose(window)) {
        for (Mesh* mesh : meshes)
            mesh->updateTransform();
        
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;       // frametime in seconds :contentReference[oaicite:1]{index=1},

        lastFrame = currentFrame;

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(g_GpuProgramID);
        glm::vec4 view_vector;
        if (isFreeCamera) {
            axis = -1.0f; // set axis to -1.0f for free camera
            // golf_club->controller = false; // Disable controller for the golf club when using free camera
            freecam->getVectors();
            freecam->setCameraAxis();
            if(!test.aim){
                freecam->setCameraAngles();
                freecam->move(window, deltaTime);
            }
            freecam->sendToGPU(view_uniform, projection_uniform); // Send the view and projection matrices to the GPU
            the_projection = freecam->getProjection();
            the_view = freecam->getView();
            view_vector = freecam->getViewVector(); // Get the view vector of the camera
        }
        else {
            axis = 1.0f;
            //golf_club->controller = true;

            

            if(!test.aim){
                ball_position = ball->getCenter();
                float cam_x = r * lookatcam->pitch;
                float cam_y = r * lookatcam->yaw;
                float cam_z = r * lookatcam->roll;
                camera_position = ball_position + glm::vec4(cam_x, cam_y, cam_z, 0.0f);
            }

            lookatcam->setCameraAngles(); // Atualiza pitch/yaw/roll com o mouse
            
            lookatcam->setPosition(camera_position);
            lookatcam->setView(ball_position); // Câmera sempre olha para a bola
            lookatcam->sendToGPU(view_uniform, projection_uniform);
            the_view = lookatcam->getView();
            the_projection = lookatcam->getProjection();
            view_vector = lookatcam->getViewVector();


        }

        ball->testCollisionWithCube(void_zone);

        ball->testCollisionWithPlane(floor); // Test collision with the floor
        for (Plane* wall : walls) {
            ball->testCollisionWithPlane(wall); // Test collision with the walls
        }
        ball->testCollisionWithCylinder(hole); // Test collision with the hole

        float m = ball->body->getMass(); // Get the mass of the ball
        glm::vec4 mg = g * m; // Calculate the gravitational force
        ball->body->addForce(mg); // Apply gravitational force to the ball

        if (test.teleport) {
            freecam->setPosition(ball->body->getPosition()); // Set the camera position to the ball's position
            test.teleport = false; // Reset the teleport flag
        }


        if (test.manualControl) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                test.dz = 1;
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                test.dz = -1;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                test.dx = -1;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                test.dx = 1;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
                test.dy = 1;
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
                test.dy = -1;
            }
            
            glm::vec4 direction = glm::vec4(test.dx, test.dy, test.dz, 0.0f); // Get the direction of the ball
            if(!isFreeCamera){
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    test.dz = -1;
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    test.dz = 1;
                }
                direction = glm::vec4(test.dx, test.dy, test.dz, 0.0f); // Get the direction of the ball
                direction = normalize(dot(view_vector, -direction)/norm(view_vector) * view_vector); // Project the direction onto the view vector
            }

            ball->body->addForce(direction); // Apply a force to the ball based on the dx, dy, dz values
            ball->body->applyTorque(20.0f * direction); // Apply a torque to the ball based on the dx, dy, dz values
            test.resetDirection(); // Reset the direction of the ball
        }
        

        float y = sin(g_CameraPhi);
        float z = cos(g_CameraPhi) * cos(g_CameraTheta);
        float x = cos(g_CameraPhi) * sin(g_CameraTheta);
        glm::vec4 direction = normalize(glm::vec4(x,y,z,0.0f));
        glm::vec4 velocity = direction * strength;
        if(test.aim && ball->isGrounded) {
            ball->body->resetForce();
            ball->body->resetAcceleration();
            ball->body->resetVelocity();
            glm::vec4 p0 = ball->getCenter();
            p0.y += 0.5f; // Slightly above the ground
            bezier_curve->setTrajectoryFromVelocity(p0, velocity, g, 1.0f);
            bezier_curve->transform = ball->getTransform(); // Get the transformation matrix of the ball


        }
        if(test.hit){
            test.hit = false;
            ball->body->setVelocity(velocity);
            ball->body->setAngularAcceleration(velocity);
            ball->isGrounded = false; // Set the grounded flag to false
        }
        // golf_club->animate(deltaTime, window); // Animate the golf club
        if(test.debug){
            bezier_curve->printInterpolatedPoints(); // Print the interpolated points of the Bezier curve
        }
        if(test.aim){
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(bezier_curve->transform));
            bezier_curve->draw(g_GpuProgramID); // Draw the Bezier curve
        }
        for (Mesh* mesh : meshes) {


            if (test.debug) {
                std::cout << "Mesh: " << mesh->getName() << ", Position: " << glm::to_string(mesh->body->getPosition()) << std::endl;
                std::cout << "Mesh: " << mesh->getName() << ", Transform: " << glm::to_string(mesh->transform) << std::endl;
                std::cout << "Mesh: " << mesh->getName() << ", Center: " << glm::to_string(mesh->getCenter()) << std::endl;

                
            }

            if (test.reset && mesh->getName() == "Ball") {
                std::cout << "Ball Velocity: " << glm::to_string(mesh->body->getVelocity()) << std::endl;
                std::cout << "Ball Angular Velocity: " << glm::to_string(mesh->body->getAngularVelocity()) << std::endl;
                std::cout << "Ball Force: " << glm::to_string(mesh->body->getForce()) << std::endl;
                std::cout << "Ball Acceleration: " << glm::to_string(mesh->body->getAcceleration()) << std::endl;
                std::cout << "Ball Position: " << glm::to_string(mesh->body->getPosition()) << std::endl;
                mesh->body->setPosition(glm::vec4(0, 0, 0, 1.0f)); // Reset the position of the mesh
                mesh->body->resetVelocity(); // Reset the velocity of the mesh
                mesh->body->resetAngularVelocity(); // Reset the angular velocity of the mesh
                mesh->body->resetForce(); // Reset the force of the mesh
                mesh->body->resetAcceleration(); // Reset the acceleration of the mesh
                test.reset = false; // Reset the reset flag
            }

            if (mesh->isTextured()) {
                glUniform1i(glGetUniformLocation(g_GpuProgramID, "use_texture"), true);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mesh->getTextureId());
                glUniform1i(glGetUniformLocation(g_GpuProgramID, "texture_diffuse"), 0);

                
                glSamplerParameteri(mesh->getTextureId(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glSamplerParameteri(mesh->getTextureId(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                if (mesh->getName() == "floor" || mesh->getName() == "roof") {
                    glSamplerParameteri(mesh->getTextureId(), GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glSamplerParameteri(mesh->getTextureId(), GL_TEXTURE_WRAP_T, GL_REPEAT);

                }

                
            }
            else {
                glUniform1i(glGetUniformLocation(g_GpuProgramID, "use_texture"), false);
            }
            mesh->sendTransform(model_uniform); // Set the transformation matrix for the mesh
            mesh->body->update(deltaTime); // Update the rigid body physics
            //if (mesh->getName() != "unit_cube") 
            virtual_scene->draw(g_GpuProgramID, mesh->getName());

        }
        test.debug = false;
        
        
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (glm::mat4 transform : cloud_transforms) {

            cloud->setTransform(transform); // Set the transformation matrix for each cloud
            cloud->sendTransform(model_uniform); // Set the transformation matrix for the cloud
            virtual_scene->draw(g_GpuProgramID, cloud->getName()); // Draw the cloud    
        }
        glDisable(GL_BLEND);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // We finalize the use of operating system resources
    glfwTerminate();

    // End of program
    return 0;
}

// Loads a Vertex Shader from a GLSL file. See definition of LoadShader() below.
GLuint LoadShader_Vertex(const char* filename) {
    // We create an identifier (ID) for this shader, informing that it will
    // be applied to vertices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // We load and compile the shader
    LoadShader(filename, vertex_shader_id);

    // Returns the ID generated above
    return vertex_shader_id;
}

// Loads a Fragment Shader from a GLSL file. See definition of LoadShader() below.
GLuint LoadShader_Fragment(const char* filename) {
    // We create an identifier (ID) for this shader, informing that it will
    // be applied to fragments.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // We load and compile the shader
    LoadShader(filename, fragment_shader_id);

    // Returns the ID generated above
    return fragment_shader_id;
}

// Helper function, used by the two functions above. Loads GPU code from
// a GLSL file and compiles it.
void LoadShader(const char* filename, GLuint shader_id) {
    // We read the text file indicated by the variable "filename"
    // and put its content in memory, pointed by the variable
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    }
    catch (std::exception& e) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>(str.length());

    // Sets the GLSL shader code, contained in the string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compiles the GLSL shader code (at runtime)
    glCompileShader(shader_id);

    // We check if any error or "warning" occurred during compilation
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // We allocate memory to store the compilation log.
    // The "new" call in C++ is equivalent to "malloc()" in C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Prints to the terminal any compilation error or "warning"
    if (log_length != 0) {
        std::string  output;

        if (!compiled_ok) {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // The "delete" call in C++ is equivalent to "free()" in C
    delete[] log;
}

// Function that loads the vertex and fragment shaders that will be
// used for rendering. See slides 180-200 of the document Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles() {
    // Note that the path to the files "shader_vertex.glsl" and
    // "shader_fragment.glsl" are fixed, assuming the existence
    // of the following structure in the file system:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (or Debug/ or Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // We delete the previous GPU program, if it exists.
    if (g_GpuProgramID != 0)
        glDeleteProgram(g_GpuProgramID);

    // We create a GPU program using the shaders loaded above.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);
}

// This function creates a GPU program, which must contain
// a Vertex Shader and a Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id) {
    // We create an identifier (ID) for this GPU program
    GLuint program_id = glCreateProgram();

    // Definition of the two GLSL shaders that must be executed by the program
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linking the shaders above to the program
    glLinkProgram(program_id);

    // We check if any error occurred during linking
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Prints to the terminal any linking error
    if (linked_ok == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // We allocate memory to store the compilation log.
        // The "new" call in C++ is equivalent to "malloc()" in C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // The "delete" call in C++ is equivalent to "free()" in C
        delete[] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Returns the ID generated above
    return program_id;
}

// Definition of the function that will be called whenever the operating system window
// is resized, consequently changing the size of the
// "framebuffer" (memory region where the image pixels are stored).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // We indicate that we want to render in the entire framebuffer region. The
    // "glViewport" function defines the mapping from "normalized device
    // coordinates" (NDC) to "pixel coordinates". This is the "Screen Mapping"
// or "Viewport Mapping" operation seen in class ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // We also update the ratio that defines the window aspect ratio (width /
    // height), which will be used in the definition of the projection matrices,
    // so that there are no distortions during the "Screen Mapping"
    // above, when NDC is mapped to pixel coordinates. See slides 205-215 of the document Aula_09_Projecoes.pdf.
    //
    // The cast to float is necessary because integer numbers are rounded when
    // divided!
    g_ScreenRatio = (float)width / height;
}

// Global variables that store the last mouse cursor position, so
// that we can calculate how much the mouse has moved between two instants
// of time. Used in the CursorPosCallback() below.
double g_LastCursorPosX, g_LastCursorPosY;

// Callback function called whenever the user presses any of the mouse buttons
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // If the user pressed the left mouse button, we store the
        // current cursor position in the variables g_LastCursorPosX and
        // g_LastCursorPosY. Also, we set the variable
        // g_LeftMouseButtonPressed to true, to know that the user is
        // currently pressing the left button.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        // When the user releases the left mouse button, we update the
        // variable below to false.
        g_LeftMouseButtonPressed = false;
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        test.aim = !test.aim;
    }
}

// Callback function called whenever the user moves the mouse cursor over
// the OpenGL window.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Below we execute the following: if the left mouse button is
    // pressed, we compute how much the mouse has moved since the last
    // instant of time, and use this movement to update the
    // parameters that define the camera position within the virtual scene.
    // Thus, the user can control the camera.

    if (!g_LeftMouseButtonPressed)
        return;
    
    // Mouse cursor displacement in x and y in screen coordinates!
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // We update camera parameters with the displacements
    g_CameraTheta -= 0.01f * dx;
    g_CameraPhi += 0.01f * dy * axis; // axis is 1.0f for lookat camera and -1.0f for free camera

    // In spherical coordinates, the phi angle must be between -pi/2 and +pi/2.
    float phimax = 3.141592f / 2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    // We update the global variables to store the current cursor position as
    // the last known cursor position.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}

// Callback function called whenever the user moves the mouse "wheel".
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // We update the camera distance to the origin using the
    // movement of the "wheel", simulating a ZOOM.
    camera_distance -= 0.1f * yoffset;

    // A look-at camera can never be exactly "on top" of the point it is
    // looking at, as this causes division by zero problems in
    // the definition of the camera coordinate system. That is, the variable below
    // can never be zero. Previous versions of this code had this bug,
    // which was detected by student Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (camera_distance < verysmallnumber)
        camera_distance = verysmallnumber;
}

// Definition of the function that will be called whenever the user presses any
// keyboard key. See http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod) {
    // ======================
    // Do not modify this loop! It is used for automated grading of
    // the labs. It must always be the first command of this KeyCallback() function.
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ======================

    // If the user presses the ESC key, we close the window.
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        test.reset = true;
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        test.teleport = true;
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        test.debug = true; // Set the debug flag to true
    }
    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
        strength += 10.0f; // Increase the strength of the hit
        std::cout << "Strength: " << strength << std::endl; // Print the current strength to the console
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
        strength -= 10.0f; // Decrease the strength of the hit
        if (strength < 0.0f) strength = 0.0f; // Ensure strength does not go negative
        std::cout << "Strength: " << strength << std::endl; // Print the current strength to the console
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        test.manualControl = !test.manualControl; // Toggle manual control
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        test.hit = true; // Set the hit flag to true
    }
    

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);



    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        isFreeCamera = !isFreeCamera;


    float delta = 3.141592 / 16; // 22.5 degrees, in radians.

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
    }


    // If the user presses the P key, we use perspective projection.
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        g_UsePerspectiveProjection = true;
    }

    // If the user presses the O key, we use orthographic projection.
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        g_UsePerspectiveProjection = false;
    }

    // If the user presses the H key, we toggle the informational text shown on the screen.
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        g_ShowInfoText = !g_ShowInfoText;
    }
}

// We define the callback for printing GLFW errors to the terminal
void ErrorCallback(int error, const char* description) {
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// We write on the screen the number of frames rendered per second (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window) {
    if (!g_ShowInfoText)
        return;

    // Static variables keep their values between subsequent calls
    // of the function!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // We retrieve the number of seconds that have passed since the program started
    float seconds = (float)glfwGetTime();

    // Number of seconds since the last fps calculation
    float ellapsed_seconds = seconds - old_seconds;

    if (ellapsed_seconds > 1.0f) {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f - (numchars + 1) * charwidth, 1.0f - lineheight, 1.0f);
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

