#ifndef _GEOMETRICS_HPP
#define _GEOMETRICS_HPP
#include "glcontext.hpp"

// We define a structure that will store the necessary data to render
// each object in the virtual scene.
//
// Structure representing a geometric model loaded from a ".obj" file.
// See https://en.wikipedia.org/wiki/Wavefront_.obj_file .
//
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

class Mesh {
protected:
    glm::mat4 transform;
    ObjModel* model; // Model data
    std::string name;
    int id;
    void ComputeNormals();
    void BuildTrianglesAndAddToVirtualScene(VirtualScene& scene);
public:
    Mesh() = delete;
    Mesh(VirtualScene& scene, std::string filename);
    ~Mesh() = default;
    inline std::string getName() { return name; }
    inline glm::mat4 getTransform() const { return transform; }


};

#endif // _GEOMETRICS_HPP