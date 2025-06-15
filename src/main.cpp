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


// Local headers, defined in the "include/" folder
#include "../include/utils.h"
#include "../include/GLFW/glfw3.h"  // Window creation for the operating system
#include "../include/matrices.hpp"
#include "../include/camera.hpp"
#include "../include/timer.hpp"
#include "../include/geometrics.hpp"
#include "../include/glcontext.hpp"

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

// "g_LeftMouseButtonPressed = true" if the user is currently pressing the left mouse button
// See MouseButtonCallback() function.


// Variables that define the camera in spherical coordinates, controlled by
// the user through the mouse (see CursorPosCallback() function). The effective
// camera position is calculated inside the main() function, within the rendering loop.

bool isFreeCamera = true;
glm::vec4 position = glm::vec4(1.0f, 0.5f, 1.0f, 1.0f); // free camera position
glm::vec4 camera_view_vector;
glm::vec4 camera_up_vector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // "up" vector fixed to point to the "sky" (global Y axis)
float speed = 1.0f;
double deltaTime = 0.0f;
bool invert_axis = false;


// Variable that controls the type of projection used: perspective or orthographic.
bool g_UsePerspectiveProjection = true;

// Variable that controls whether the informational text will be shown on the screen.
bool g_ShowInfoText = true;

// Variables that define a GPU program (shaders). See LoadShadersFromFiles() function.
GLuint g_GpuProgramID = 0;

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
    GLint render_as_black_uniform = glGetUniformLocation(g_GpuProgramID, "render_as_black"); // Boolean variable in shader_vertex.glsl

    // We enable the Z-buffer. See slides 104-116 of the document Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Auxiliary variables used for calling the
    // TextRendering_ShowModelViewProjection() function, storing 4x4 matrices.
    glm::mat4 the_projection;
    glm::mat4 the_model;
    glm::mat4 the_view;

    // We stay in an infinite loop, rendering, until the user closes the window
    double lastFrame = glfwGetTime();
    Freecam *freecam = new Freecam();
    VirtualScene *virtual_scene = new VirtualScene();
    Cube *cube = new Cube();
    virtual_scene->addFromMesh(*cube);

    while (!glfwWindowShouldClose(window)) {

        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;       // frametime in seconds :contentReference[oaicite:1]{index=1}
        lastFrame = currentFrame; 



        // Here we execute the rendering operations

        // We set the "background" color of the framebuffer to white. This color is
        // defined as RGBA coefficients: Red, Green, Blue, Alpha; that is:
        // Red, Green, Blue, Alpha (transparency value).
        // We will talk about color systems in the Lighting Models classes.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // We "paint" all the pixels of the framebuffer with the color defined above,
        // and also reset all the pixels of the Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // We ask the GPU to use the GPU program created above (containing
        // the vertex and fragment shaders).
        glUseProgram(g_GpuProgramID);

        // Below we define the variables that effectively define the virtual camera.
        // See slides 195-227 and 229-234 of the document Aula_08_Sistemas_de_Coordenadas.pdf.
    
        if (isFreeCamera) {
        
            freecam->setCameraAngles();
            freecam->getVectors();
            freecam->setCameraAxis();
            freecam->move(window, deltaTime);

        }
       /*  else {
            x *= r;
            y *= r;
            z *= r;
            camera_position_c = glm::vec4(x, y, z, 1.0f); // Point "c", camera center
            camera_view_vector = camera_lookat_l - camera_position_c; // "view" vector, direction the camera is facing
        }
 */
        // We compute the "View" matrix using the camera parameters to
        // define the camera coordinate system. See slides 2-14, 184-190, and 236-242 of the document Aula_08_Sistemas_de_Coordenadas.pdf.
    
    

        // We send the "view" and "projection" matrices to the video card
        // (GPU). See the file "shader_vertex.glsl", where these are
        // effectively applied to all points.
        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(freecam->getView()));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(freecam->getProjection()));

        
        the_projection = freecam->getProjection();
        the_view = freecam->getView();


        // We will draw 3 instances (copies) of the cube
        for (int i = 1; i <= 3; ++i) {
            // Each copy of the cube has an independent modeling matrix,
            // since each copy will be in a different position (rotation, scale, ...)
            // relative to the global space (World Coordinates). See
            // slides 2-14 and 184-190 of the document Aula_08_Sistemas_de_Coordenadas.pdf.
            glm::mat4 model;

            if (i == 1) {
                // The first copy of the cube will not undergo any modeling
                // transformation. Therefore, its "model" matrix is the identity, and
                // its coordinates in global space (World Coordinates) will be
                // *exactly the same* as its coordinates in model space
                // (Model Coordinates).
                model = Matrix_Identity();
            }
            else if (i == 2) {
                // The second copy of the cube will undergo a non-uniform scaling,
                // followed by a rotation on the axis (1,1,1), and a translation in Z (in this order!).
                model = Matrix_Translate(0.0f, 0.0f, -2.0f) // THIRD translation
                    * Matrix_Rotate(3.141592f / 8.0f, glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)) // SECOND rotation
                    * Matrix_Scale(2.0f, 0.5f, 0.5f); // FIRST scale
            }
            else if (i == 3) {
                // The third copy of the cube will undergo rotations in X, Y, and Z (in this
                // order) following the Euler angles system, and then a
                // translation in X. See slides 106-107 of the document Aula_07_Transformacoes_Geometricas_3D.pdf.
                model = Matrix_Translate(-2.0f, 0.0f, 0.0f) // FOURTH translation
                    * Matrix_Rotate_Z(g_AngleZ)  // THIRD Euler Z rotation
                    * Matrix_Rotate_Y(g_AngleY)  // SECOND Euler Y rotation
                    * Matrix_Rotate_X(g_AngleX); // FIRST Euler X rotation

                // We store the model, view, and projection matrices of the third cube
                // to show them on the screen through the function TextRendering_ShowModelViewProjection().
                the_model = model;
                the_view = freecam->getView();
                the_projection = freecam->getProjection();
            }

            // We send the "model" matrix to the video card (GPU). See the
            // file "shader_vertex.glsl", where this is effectively
            // applied to all points.
            glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

            // We inform the video card (GPU) that the boolean variable
            // "render_as_black" should be set to "false". See the file
            // "shader_vertex.glsl".
            glUniform1i(render_as_black_uniform, false);

            // We ask the GPU to rasterize the cube vertices pointed by the
            // VAO as triangles, forming the cube faces. This
            // rendering will execute the Vertex Shader defined in the file
            // "shader_vertex.glsl", and it will use the matrices
            // "model", "view" and "projection" defined above and already sent
            // to the video card (GPU).
            //
            // See the definition of g_VirtualScene["cube_faces"] inside the
            // BuildTriangles() function, and see the documentation of the
            // glDrawElements() function at http://docs.gl/gl3/glDrawElements.


            //We need to make a generic VAO later, so we can use the same VAO for all meshes
            glBindVertexArray(cube->getVAO());
            virtual_scene->drawScene(&render_as_black_uniform);




            // We draw a point of size 15 pixels on top of the third vertex
            // of the third cube. This vertex has model coordinates equal to
            // (0.5, 0.5, 0.5, 1.0).
            if (i == 3) {
                glPointSize(15.0f);
                glDrawArrays(GL_POINTS, 3, 1);
            }
        }

        // Now we want to draw the global XYZ axes.
        // To do this, we set the modeling matrix to the identity.
        // See slides 2-14 and 184-190 of the document Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 model = Matrix_Identity();

        // We send the new "model" matrix to the video card (GPU). See the
        // file "shader_vertex.glsl".
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));

    

        // We take a vertex with model coordinates (0.5, 0.5, 0.5, 1) and pass it
        // through all coordinate systems stored in the
        // the_model, the_view, and the_projection matrices; and write on the screen
        // the matrices and resulting points of these transformations.
        glm::vec4 p_model(0.5f, 0.5f, 0.5f, 1.0f);


        /*
        TextRendering_ShowModelViewProjection(window, the_projection, the_view, the_model, p_model);
        TextRendering_ShowEulerAngles(window);
        TextRendering_ShowProjection(window);
        TextRendering_ShowFramesPerSecond(window);
       */

        glfwSwapBuffers(window);

        // We check with the operating system if there was any user interaction
        // (keyboard, mouse, ...). If so, the callback functions
        // previously defined using glfwSet*Callback() will be called
        // by the GLFW library.
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
    g_CameraTheta -= 0.01f * dx * camera_axis.x;
    g_CameraPhi += 0.01f * dy * camera_axis.y;

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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
        invert_axis = !invert_axis;

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        isFreeCamera = !isFreeCamera;
    // The code below implements the following logic:
    //   If press key X       then g_AngleX += delta;
    //   If press shift+X key then g_AngleX -= delta;
    //   If press key Y       then g_AngleY += delta;
    //   If press shift+Y key then g_AngleY -= delta;
    //   If press key Z       then g_AngleZ += delta;
    //   If press shift+Z key then g_AngleZ -= delta;

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



    // If the user presses the space key, we reset the Euler angles to zero.
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        g_AngleX = 0.0f;
        g_AngleY = 0.0f;
        g_AngleZ = 0.0f;
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

// This function receives a vertex with model coordinates p_model and passes it
// through all coordinate systems stored in the model,
// view, and projection matrices; and writes on the screen the matrices and resulting
// points of these transformations.
void TextRendering_ShowModelViewProjection(
    GLFWwindow* window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
) {
    if (!g_ShowInfoText)
        return;

    glm::vec4 p_world = model * p_model;
    glm::vec4 p_camera = view * p_world;
    glm::vec4 p_clip = projection * p_camera;
    glm::vec4 p_ndc = p_clip;
    if (p_clip.w)
        p_ndc = p_clip / p_clip.w;

    float pad = TextRendering_LineHeight(window);

    TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f - pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f - 2 * pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f - 6 * pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f - 7 * pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f - 8 * pad, 1.0f);

    TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f - 9 * pad, 1.0f);
    TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f - 10 * pad, 1.0f);

    TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f - 14 * pad, 1.0f);
    TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f - 15 * pad, 1.0f);
    TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f - 16 * pad, 1.0f);

    TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f - 17 * pad, 1.0f);
    TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f - 18 * pad, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glm::vec2 a = glm::vec2(-1, -1);
    glm::vec2 b = glm::vec2(+1, +1);
    glm::vec2 p = glm::vec2(0, 0);
    glm::vec2 q = glm::vec2(width, height);

    glm::mat4 viewport_mapping = Matrix(
        (q.x - p.x) / (b.x - a.x), 0.0f, 0.0f, (b.x * p.x - a.x * q.x) / (b.x - a.x),
        0.0f, (q.y - p.y) / (b.y - a.y), 0.0f, (b.y * p.y - a.y * q.y) / (b.y - a.y),
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f - 22 * pad, 1.0f);
    TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f - 23 * pad, 1.0f);
    TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f - 24 * pad, 1.0f);

    TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f - 25 * pad, 1.0f);
    TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f - 26 * pad, 1.0f);
}

// We write on the screen the Euler angles defined in the global variables
// g_AngleX, g_AngleY, and g_AngleZ.
void TextRendering_ShowEulerAngles(GLFWwindow* window) {
    if (!g_ShowInfoText)
        return;

    float pad = TextRendering_LineHeight(window);

    char buffer[80];
    snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);

    TextRendering_PrintString(window, buffer, -1.0f + pad / 10, -1.0f + 2 * pad / 10, 1.0f);
}

// We write on the screen which projection matrix is being used.
void TextRendering_ShowProjection(GLFWwindow* window) {
    if (!g_ShowInfoText)
        return;

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    if (g_UsePerspectiveProjection)
        TextRendering_PrintString(window, "Perspective", 1.0f - 13 * charwidth, -1.0f + 2 * lineheight / 10, 1.0f);
    else
        TextRendering_PrintString(window, "Orthographic", 1.0f - 13 * charwidth, -1.0f + 2 * lineheight / 10, 1.0f);
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

