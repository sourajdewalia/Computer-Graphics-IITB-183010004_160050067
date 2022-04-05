#include "obj_model.hpp"
#include "gl_framework.hpp"

ObjModel::ObjModel(std::string fileName)
{
    Assimp::Importer importer;
    scene = (aiScene *)importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    vertexCount = 0;
    int meshIndex = scene->mRootNode->mChildren[0]->mMeshes[0];
    int faceCount = scene->mMeshes[meshIndex]->mNumFaces;
    glBufferData(GL_ARRAY_BUFFER, faceCount * 3 * sizeof(glm::vec4) + faceCount * 3 * sizeof(glm::vec4) + faceCount * 3 * sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
    for (int i = 0; i < faceCount; i++)
    {
        int v1 = scene->mMeshes[meshIndex]->mFaces[i].mIndices[0];
        int v2 = scene->mMeshes[meshIndex]->mFaces[i].mIndices[1];
        int v3 = scene->mMeshes[meshIndex]->mFaces[i].mIndices[2];

        auto p_temp1 = scene->mMeshes[meshIndex]->mVertices[v1];
        auto p_temp2 = scene->mMeshes[meshIndex]->mVertices[v2];
        auto p_temp3 = scene->mMeshes[meshIndex]->mVertices[v3];

        v_pos.push_back(glm::vec4(p_temp1[0], p_temp1[1], p_temp1[2], 1.0));
        v_pos.push_back(glm::vec4(p_temp2[0], p_temp2[1], p_temp2[2], 1.0));
        v_pos.push_back(glm::vec4(p_temp3[0], p_temp3[1], p_temp3[2], 1.0));

        // std::cout << "\n"
        // << v_pos[v_pos.size() - 3][0] << " " << v_pos[v_pos.size() - 3][1] << " " << v_pos[v_pos.size() - 3][2];
        // std::cout << "\n"
        // << v_pos[v_pos.size() - 2][0] << " " << v_pos[v_pos.size() - 2][1] << " " << v_pos[v_pos.size() - 2][2];
        // std::cout << "\n"
        // << v_pos[v_pos.size() - 1][0] << " " << v_pos[v_pos.size() - 1][1] << " " << v_pos[v_pos.size() - 1][2];
        auto n_temp1 = scene->mMeshes[meshIndex]->mNormals[v1];
        auto n_temp2 = scene->mMeshes[meshIndex]->mNormals[v2];
        auto n_temp3 = scene->mMeshes[meshIndex]->mNormals[v3];

        v_normal.push_back(glm::vec4(n_temp1[0], n_temp1[1], n_temp1[2], 1.0));
        v_normal.push_back(glm::vec4(n_temp2[0], n_temp2[1], n_temp2[2], 1.0));
        v_normal.push_back(glm::vec4(n_temp3[0], n_temp3[1], n_temp3[2], 1.0));

        auto t_temp1 = scene->mMeshes[meshIndex]->mTextureCoords[0][v1];
        auto t_temp2 = scene->mMeshes[meshIndex]->mTextureCoords[0][v2];
        auto t_temp3 = scene->mMeshes[meshIndex]->mTextureCoords[0][v3];

        v_tex.push_back(glm::vec2(t_temp1.x, t_temp1.y));
        v_tex.push_back(glm::vec2(t_temp2.x, t_temp2.y));
        v_tex.push_back(glm::vec2(t_temp3.x, t_temp3.y));
    }

    vertexCount = v_pos.size();
    glBufferSubData(GL_ARRAY_BUFFER, 0, v_pos.size() * sizeof(v_pos[0]), v_pos.data());
    glBufferSubData(GL_ARRAY_BUFFER, v_pos.size() * sizeof(v_pos[0]), v_normal.size() * sizeof(v_normal[0]), v_normal.data());
    glBufferSubData(GL_ARRAY_BUFFER, 2 * v_pos.size() * sizeof(v_pos[0]), v_tex.size() * sizeof(v_tex[0]), v_tex.data());

    int matIndex = scene->mMeshes[meshIndex]->mMaterialIndex;
    aiMaterial *mtl = scene->mMaterials[matIndex];
    aiString texPath;
    if (AI_SUCCESS == mtl->GetTexture(aiTextureType_AMBIENT, 0, &texPath))
    {
        textureGiven = true;
        std::string texPath_ = "../images/" + std::string(texPath.data);
        tex = LoadTextureAdv(texPath_.c_str());
        glBindTexture(GL_TEXTURE_2D, tex);
        std::cout << "\nTexture file : " << texPath.data;
    }
    else
    {
        textureGiven = false;
    }

    aiColor4D diffuse;
    aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
    aiColor4D ambient;
    aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &ambient);
    aiColor4D specular;
    aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &specular);
    aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &maxSpec);

    std::string vertexShaderFile("s_vshader.glsl");
    std::string fragmentShaderFile("s_fshader.glsl");

    std::vector<GLuint> shaderList;
    shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertexShaderFile));
    shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragmentShaderFile));
    shaderProgram = csX75::CreateProgramGL(shaderList);
    glUseProgram(shaderProgram);

    GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
    //    GLuint vColor = glGetAttribLocation(shaderProgram, "vColor");
    GLuint vNormal = glGetAttribLocation(shaderProgram, "vNormal");
    GLuint vTexCoords = glGetAttribLocation(shaderProgram, "texCord");
    uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
    normalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
    viewMatrix = glGetUniformLocation(shaderProgram, "viewMatrix");

    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    //    glEnableVertexAttribArray(vColor);
    //    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(v_positions)));

    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(faceCount * 3 * sizeof(v_pos[0])));

    glEnableVertexAttribArray(vTexCoords);
    glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(faceCount * 2 * 3 * sizeof(v_pos[0])));
}

void ObjModel::render(glm::mat4 viewMatrix_, glm::mat4 modelViewMatrix_, glm::mat3 normalMatrix_)
{
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix_));
    glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelViewMatrix_));
    glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix_));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
