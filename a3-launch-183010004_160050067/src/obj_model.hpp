#ifndef _OBJ_MODEL_HPP_
#define _OBJ_MODEL_HPP_

#include "assimp/include/assimp/defs.h"
#include "assimp/include/assimp/texture.h"
#include "assimp/include/assimp/Importer.hpp"
#include "assimp/include/assimp/scene.h"
#include "assimp/include/assimp/postprocess.h"
#include <string>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "texture.hpp"
#include "shader_util.hpp"
//#include "gl_framework.hpp"

class ObjModel
{
public:
    std::vector<glm::vec4> v_pos;
    std::vector<glm::vec4> v_normal;
    std::vector<glm::vec2> v_tex;
    std::vector<std::string> texFilenames;
    aiScene *scene;

    GLuint vao;
    GLuint vbo;
    int vertexCount;

    GLuint uModelViewMatrix;
    GLuint viewMatrix;
    GLuint normalMatrix;
    GLuint tex;
    GLuint shaderProgram;

    glm::vec4 ka;
    glm::vec4 kd;
    glm::vec4 ks;
    GLfloat shininess;
    unsigned int maxSpec;

    bool textureGiven;

    ObjModel(std::string fileName);
    void render(glm::mat4, glm::mat4, glm::mat3);
};

#endif
