/*
  CSX75 Assignment 2

  Modified from An Introduction to OpenGL Programming,
  Ed Angel and Dave Shreiner, SIGGRAPH 2013

  Modified from Example 3 Written by Parag Chaudhuri, 2015
*/

#include "assignment_02.hpp"

/* Constants for VCS to be read from scene file */
glm::vec4 eyeCord;
glm::vec4 LCord;
glm::vec4 UCord;
std::vector<float> lrtb;
float near;
float far;
float lf, rf, tf, bf;

float Rw = 2.0, Lw = -2.0, Tw = 1.5, Bw = -1.5;

/* Constants defining grid specification */
const int gridSize = 50;
const int maxGridSize = 100;
//float edge = 1.0f / (gridSize - 1);
//const int numVertices = (gridSize - 1)*(gridSize)*(gridSize)*6;
int numVertices = 0;
int state = 0;

GLuint shaderProgram, shaderGrid;
GLuint vaoBox, vboBox;
GLuint vaoEye, vboEye, vaoLines, vboLines, vaoFrustum, vboFrustum;
std::vector<GLuint> vaoVec;
std::vector<GLuint> vboVec;
std::vector<GLuint> vColor;
std::vector<GLuint> vPosition;
std::vector<GLuint> frusColor;
std::vector<GLuint> frusPosition;

glm::mat4 rotation_matrix;
glm::mat4 modelRotMatrix;
glm::mat4 modelScaleMatrix;
glm::mat4 modelTrMatrix;
glm::mat4 ortho_matrix;
glm::mat4 modelview_matrix;

glm::vec4 eyeColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
glm::vec4 projColor = glm::vec4(1.0, 0.0, 1.0, 1.0);
glm::vec4 frustColor = glm::vec4(0.0, 1.0, 1.0, 0.3);

glm::mat4 orthoMatrix1 = glm::ortho(-200.0, 200.0, -200.0, 200.0, -200.0, 200.0),
          orthoMatrix2 = glm::ortho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0),
          orthoMatrix,
          translationMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(-50.f, -50.f, -50.f)),
          translationMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f)),
          translationMatrix,
          translationMatrixCursor,
          translationMatrixBoxes,
          rotationMatrix,
          vcsToWcs,
          wcsToVcs,
          vcsToCcs,
          ccsToNdcs,
          vcsToNdcs,
          ndcsToDcs,
          modelViewMatrix,
          tempMatrix,
          csMatrix,
          dcsMatrix;
GLint ndcsBool = 0;

/* Uniform data to be passed to shader */
GLuint  uModelViewMatrixGrid,
        uModelViewMatrixModel,
        uCsMatrixGrid,
        uCsMatrixModel,
        uNdcsFlagGrid,
        uNdcsFlagModel,
        uDcsMatrixGrid,
        uDcsMatrixModel;

/* Grid vertices array */
glm::vec4 vPosBox[(gridSize - 1) * (gridSize) * (gridSize) * 6];

struct Models
{
    std::string modelName;
    std::vector<float> scale;
    std::vector<float> rot;
    std::vector<float> tr;
    int vertexCount;
    glm::mat4 transformMatrix;
};

std::vector<Models> models;

//-----------------------------------------------------------------

//6 faces, 2 triangles/face, 3 vertices/triangle
const int num_vertices = 36;

//Eight vertices in homogenous coordinates
glm::vec4 positions[8];

glm::vec4 projPositions[8];
glm::vec4 planePositions[12];

//RGBA colors
glm::vec4 colors[8];

glm::vec4 projColors[8];
glm::vec4 planeColors[12];

int tri_idx = 0;
glm::vec4 v_positions[num_vertices];
glm::vec4 v_colors[num_vertices];

void fillPositions(int x, int y, int z)
{
    positions[0] = glm::vec4(x, y, (z + 1), 1.0);
    positions[1] = glm::vec4(x, (y + 1), (z + 1), 1.0);
    positions[2] = glm::vec4((x + 1), (y + 1), (z + 1), 1.0);
    positions[3] = glm::vec4((x + 1), y, (z + 1), 1.0);
    positions[4] = glm::vec4(x, y, z, 1.0);
    positions[5] = glm::vec4(x, (y + 1), z, 1.0);
    positions[6] = glm::vec4((x + 1), (y + 1), z, 1.0);
    positions[7] = glm::vec4((x + 1), y, z, 1.0);
}

// quad generates two triangles for each face and assigns colors to the vertices
void quad(int a, int b, int c, int d)
{
    v_positions[tri_idx] = positions[a];
    tri_idx++;
    v_positions[tri_idx] = positions[b];
    tri_idx++;
    v_positions[tri_idx] = positions[c];
    tri_idx++;
    v_positions[tri_idx] = positions[a];
    tri_idx++;
    v_positions[tri_idx] = positions[c];
    tri_idx++;
    v_positions[tri_idx] = positions[d];
    tri_idx++;
}

// generate 12 triangles: 36 vertices and 36 colors
void fillCube(void)
{
    tri_idx = 0;
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

void fillColor(float r, float g, float b){
    for(int iter=0; iter < 36; iter++){
        v_colors[iter] = glm::vec4(r, g, b, 1.0);
    }
}

/* Generate an array of coupled points for grid generation */
void generateGrid(void)
{
  int idx = 0;
  for (int i = 0; i <= maxGridSize; i+=10)
    for (int j = 0; j <= maxGridSize; j+=10)
      for (int k = 0; k <= maxGridSize; k+=10){
        if (i + 10 <= maxGridSize){
		  vPosBox[idx++] = glm::vec4((i ), (j ), (k ), 1.0);
          vPosBox[idx++] = glm::vec4((i + 10), (j), (k ), 1.0);
        }
        if (j + 10 <= maxGridSize){
          vPosBox[idx++] = glm::vec4((i ), (j ), (k ), 1.0);
          vPosBox[idx++] = glm::vec4((i ), ((j+10) ), (k ), 1.0);
        }
        if (k + 10 <= maxGridSize){
          vPosBox[idx++] = glm::vec4((i ), (j ), (k ), 1.0);
          vPosBox[idx++] = glm::vec4((i ), (j ), ((k+10) ), 1.0);
        }
      }
  numVertices = idx;
}


//-----------------------------------------------------------------

/* Function to read the scene file */
int readFile(std::string fileName)
{
    std::ifstream file;
    file.open(fileName);

    if (!file.is_open())
    {
        std::cout << "\n Invalid scene file!!!" << std::endl;
        return -1;
    }

    bool readingModel = true;
    int state = 0;
    std::string lastRead;
    Models tempModel;
    bool first = true;
    while (!file.eof())
    {
        std::string lineIn;
        lastRead = lineIn;
        getline(file, lineIn);
        std::istringstream stream(lineIn);
        std::string s;
        if (lineIn.size() == 0)
            continue;
        int count = 0;
        while (lineIn[count] == ' ')
            count++;
        if (lineIn[count] == '#')
            continue;
        if (readingModel)
        {
            //std::cout<<"\n"<<lineIn.size();
            while (getline(stream, s, ' '))
            {
                if (s == "<")
                    continue;
                if (s == ">")
                    break;
                if (s[0] == '<')
                {
                    s = s.substr(1);
                }
                if (s[s.size() - 1] == '>')
                {
                    s = s.substr(0, s.size() - 1);
                }
                if (state == 0)
                {
                    if (s[s.size() - 3] == 'r' && s[s.size() - 2] == 'a' && s[s.size() - 1] == 'w')
                    {
                        tempModel.modelName = s;
                        state = 1;
                        break;
                    }
                    else
                    {
                        readingModel = false;
                        break;
                    }
                }
                else if (state == 1)
                {
                    tempModel.scale = std::vector<float>(0);
                    float sx = atof(s.c_str());
                    tempModel.scale.push_back(sx);
                    getline(stream, s, ' ');
                    float sy = atof(s.c_str());
                    tempModel.scale.push_back(sy);
                    getline(stream, s, ' ');
                    float sz = atof(s.c_str());
                    tempModel.scale.push_back(sz);
                    state = 2;
                    break;
                }
                else if (state == 2)
                {
                    tempModel.rot = std::vector<float>(0);
                    float sx = atof(s.c_str());
                    tempModel.rot.push_back(sx);
                    getline(stream, s, ' ');
                    float sy = atof(s.c_str());
                    tempModel.rot.push_back(sy);
                    getline(stream, s, ' ');
                    float sz = atof(s.c_str());
                    tempModel.rot.push_back(sz);
                    state = 3;
                    break;
                }
                else if (state == 3)
                {
                    tempModel.tr = std::vector<float>(0);
                    float sx = atof(s.c_str());
                    tempModel.tr.push_back(sx);
                    getline(stream, s, ' ');
                    float sy = atof(s.c_str());
                    tempModel.tr.push_back(sy);
                    getline(stream, s, ' ');
                    float sz = atof(s.c_str());
                    tempModel.tr.push_back(sz);
                    models.push_back(tempModel);
                    state = 0;
                    break;
                }
            }
        }
        if (!readingModel)
        {
            //std::cout<<"\n"<<lineIn.size();
            if(not first)
                getline(stream, s, ' ');
            do
            {
                if (s == "<")
                    continue;
                if (s == ">")
                    break;
                if (s[0] == '<')
                {
                    s = s.substr(1);
                }
                if (s[s.size() - 1] == '>')
                {
                    s = s.substr(0, s.size() - 1);
                }
                if (state == 0)
                {
                    float xCord = atof(s.c_str());
                    getline(stream, s, ' ');
                    float yCord = atof(s.c_str());
                    getline(stream, s, ' ');
                    float zCord = atof(s.c_str());
                    eyeCord = glm::vec4(xCord, yCord, zCord, 1.0);
                    state = 1;
                    std::cout<<"\nThe eye found is : "<<xCord<<" "<<yCord<<" "<<zCord;
                    first = false;
                    break;
                }
                if (state == 1)
                {
                    float xCord = atof(s.c_str());
                    getline(stream, s, ' ');
                    float yCord = atof(s.c_str());
                    getline(stream, s, ' ');
                    float zCord = atof(s.c_str());
                    LCord = glm::vec4(xCord, yCord, zCord, 1.0);
                    std::cout<<"\nThe eye found is : "<<xCord<<" "<<yCord<<" "<<zCord;
                    state = 2;
                    break;
                }
                if (state == 2)
                {
                    float xCord = atof(s.c_str());
                    getline(stream, s, ' ');
                    float yCord = atof(s.c_str());
                    getline(stream, s, ' ');
                    float zCord = atof(s.c_str());
                    UCord = glm::vec4(xCord, yCord, zCord, 1.0);
                    std::cout<<"\nThe eye found is : "<<xCord<<" "<<yCord<<" "<<zCord;
                    state = 3;
                    break;
                }
                if (state == 3)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        float val = atof(s.c_str());
                        std::cout<<"\n"<<val;
                        lrtb.push_back(val);
                        getline(stream, s, ' ');
                    }
                    state = 4;
                    break;
                }
                if (state == 4)
                {
                    near = atof(s.c_str());
                    getline(stream, s, ' ');
                    far = atof(s.c_str());
                    state = 5;
                    break;
                }
            } while (getline(stream, s, ' '));
        }
    }
    std::cout << models.size();
}

int readModel(int index)
{
    std::string modelName = models[index].modelName;
    std::ifstream file;
    file.open("../data/" + modelName);
    if (!file.is_open())
    {
        std::cout << "\nInvalid model name..!!" << std::endl;
        return -1;
    }
    int count = 0;
    std::vector<int> xCord;
    std::vector<int> yCord;
    std::vector<int> zCord;
    std::vector<float> redColor;
    std::vector<float> greenColor;
    std::vector<float> blueColor;
    while (!file.eof())
    {
        std::string lineIn;
        getline(file, lineIn);
        std::istringstream stream(lineIn);
        std::string s;
        int x, y, z;
        getline(stream, s, ' ');
        x = atoi(s.c_str());
        getline(stream, s, ' ');
        y = atoi(s.c_str());
        getline(stream, s, ' ');
        z = atoi(s.c_str());
        xCord.push_back(x);
        yCord.push_back(y);
        zCord.push_back(z);
        float r, g, b;
        getline(stream, s, ' ');
        r = atof(s.c_str());
        getline(stream, s, ' ');
        g = atof(s.c_str());
        getline(stream, s, ' ');
        b = atof(s.c_str());
        redColor.push_back(r);
        greenColor.push_back(g);
        blueColor.push_back(b);
    }
    int pointCount = xCord.size();
    models[index].vertexCount = pointCount * 36;
    glBufferData(GL_ARRAY_BUFFER, 2*pointCount*sizeof(v_positions), NULL, GL_STATIC_DRAW);
    for(int iter=0; iter < xCord.size(); iter++){
        fillPositions(xCord[iter], yCord[iter], zCord[iter]);
        fillCube();
        glBufferSubData(GL_ARRAY_BUFFER, iter*sizeof(v_positions), sizeof(v_positions), v_positions);
    }
    for(int iter=0; iter < xCord.size(); iter++){
        fillColor(redColor[iter], greenColor[iter], blueColor[iter]);
        glBufferSubData(GL_ARRAY_BUFFER, (iter + pointCount)*sizeof(v_colors), sizeof(v_colors), v_colors);
    }
	return pointCount;
}

/* Initialises buffers for Grid */
void initGrid(void){
	generateGrid();

	/* Ask GL for a Vertex Attribute Object (vao)*/
	glGenVertexArrays(1, &vaoBox);
	/* Set it as the current array to be used by binding it*/
	glBindVertexArray(vaoBox);

	/* Ask GL for a Vertex Buffer Object (vbo)*/
	glGenBuffers(1, &vboBox);
	/*Set it as the current buffer to be used by binding it*/
	glBindBuffer(GL_ARRAY_BUFFER, vboBox);
	/*Copy the points into the current buffer*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPosBox), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vPosBox), vPosBox);

	/* Load shaders and use the resulting shader program*/
	std::string vertex_shader_file("vshaderGrid.glsl");
	std::string fragment_shader_file("fshaderModel.glsl");

	std::vector<GLuint> shaderList;
	shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
	shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

	shaderGrid = csX75::CreateProgramGL(shaderList);
	glUseProgram(shaderGrid);

	/* set up vertex arrays */
	GLuint vPosition = glGetAttribLocation(shaderGrid, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	uModelViewMatrixGrid = glGetUniformLocation(shaderGrid, "uModelViewMatrix");
	uCsMatrixGrid = glGetUniformLocation(shaderGrid, "csMatrix");
	uDcsMatrixGrid = glGetUniformLocation(shaderGrid, "dcs");
	uNdcsFlagGrid = glGetUniformLocation(shaderGrid, "NDCS");
}

void computeProj(){
    lf = lrtb[0]*far/near;
    rf = lrtb[1]*far/near;
    tf = lrtb[2]*far/near;
    bf = lrtb[3]*far/near;
    std::cout<<"\n lrtb : "<<lf<<" "<<rf<<" "<<tf<<" "<<bf;
    for(int iter=0; iter<4; iter++){
        projPositions[iter*2] = glm::vec4(0.0, 0.0, 0.0, 1.0);
    }
    projPositions[1] = glm::vec4(-lf, tf, -far, 1.0);
    projPositions[3] = glm::vec4(rf, tf, -far, 1.0);
    projPositions[5] = glm::vec4(-lf, -bf, -far, 1.0);
    projPositions[7] = glm::vec4(rf, -bf, -far, 1.0);
    for(int iter=0; iter<8; iter++){
        projColors[iter] = projColor;
    }

    planePositions[0] = glm::vec4(-lf, -bf, -far, 1.0);
    planePositions[1] = glm::vec4(-lf, tf, -far, 1.0);
    planePositions[2] = glm::vec4(rf, tf, -far, 1.0);
    planePositions[3] = glm::vec4(-lf, -bf, -far, 1.0);
    planePositions[4] = glm::vec4(rf, tf, -far, 1.0);
    planePositions[5] = glm::vec4(rf, -bf, -far, 1.0);

    planePositions[6] = glm::vec4(-lrtb[0], -lrtb[3], -near, 1.0);
    planePositions[7] = glm::vec4(-lrtb[0], lrtb[2], -near, 1.0);
    planePositions[8] = glm::vec4(lrtb[1], lrtb[2], -near, 1.0);
    planePositions[9] = glm::vec4(-lrtb[0], -lrtb[3], -near, 1.0);
    planePositions[10] = glm::vec4(lrtb[1], lrtb[2], -near, 1.0);
    planePositions[11] = glm::vec4(lrtb[1], -lrtb[3], -near, 1.0);
    for(int iter=0; iter<12; iter++){
        planeColors[iter] = frustColor;
    }
}

void initFrustum(void){
    // For drawing eye
    
    glEnable(GL_PROGRAM_POINT_SIZE); 

	/* Ask GL for a Vertex Attribute Object (vao)*/
	glGenVertexArrays(1, &vaoEye);
	/* Set it as the current array to be used by binding it*/
	glBindVertexArray(vaoEye);

	/* Ask GL for a Vertex Buffer Object (vbo)*/
	glGenBuffers(1, &vboEye);
	/*Set it as the current buffer to be used by binding it*/
	glBindBuffer(GL_ARRAY_BUFFER, vboEye);
	/*Copy the points into the current buffer*/
	glBufferData(GL_ARRAY_BUFFER, 2*sizeof(eyeCord), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(eyeCord), glm::value_ptr(eyeCord));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(eyeCord), sizeof(eyeColor), glm::value_ptr(eyeColor));

    glUseProgram(shaderProgram);
    GLuint fP, fC;
	frusPosition.push_back(fP);
	frusColor.push_back(fC);
	frusPosition[0] = glGetAttribLocation(shaderProgram, "vPosition");
	glEnableVertexAttribArray(frusPosition[0]);
	glVertexAttribPointer(frusPosition[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	frusColor[0] = glGetAttribLocation(shaderProgram, "vColor");
	glEnableVertexAttribArray(frusColor[0]);
	glVertexAttribPointer(frusColor[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(eyeCord)));
	uModelViewMatrixModel = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
	uCsMatrixModel = glGetUniformLocation(shaderProgram, "csMatrix");
	uDcsMatrixModel = glGetUniformLocation(shaderProgram, "dcs");
	uNdcsFlagModel = glGetUniformLocation(shaderProgram, "NDCS");

	/* Ask GL for a Vertex Attribute Object (vao)*/
	glGenVertexArrays(1, &vaoLines);
	/* Set it as the current array to be used by binding it*/
	glBindVertexArray(vaoLines);

	/* Ask GL for a Vertex Buffer Object (vbo)*/
	glGenBuffers(1, &vboLines);
	/*Set it as the current buffer to be used by binding it*/
	glBindBuffer(GL_ARRAY_BUFFER, vboLines);
	/*Copy the points into the current buffer*/
    computeProj();
	glBufferData(GL_ARRAY_BUFFER, 2*sizeof(projPositions), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(projPositions), projPositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(projPositions), sizeof(projColors), projColors);

    glUseProgram(shaderProgram);
    GLuint pP, pC;
	frusPosition.push_back(pP);
	frusColor.push_back(pC);
	frusPosition[1] = glGetAttribLocation(shaderProgram, "vPosition");
	glEnableVertexAttribArray(frusPosition[1]);
	glVertexAttribPointer(frusPosition[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	frusColor[1] = glGetAttribLocation(shaderProgram, "vColor");
	glEnableVertexAttribArray(frusColor[1]);
	glVertexAttribPointer(frusColor[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(projPositions)));
	uModelViewMatrixModel = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
	uCsMatrixModel = glGetUniformLocation(shaderProgram, "csMatrix");
	uDcsMatrixModel = glGetUniformLocation(shaderProgram, "dcs");
	uNdcsFlagModel = glGetUniformLocation(shaderProgram, "NDCS");

	/* Ask GL for a Vertex Attribute Object (vao)*/
	glGenVertexArrays(1, &vaoFrustum);
	/* Set it as the current array to be used by binding it*/
    glBindVertexArray(vaoFrustum);

	/* Ask GL for a Vertex Buffer Object (vbo)*/
	glGenBuffers(1, &vboFrustum);
	/*Set it as the current buffer to be used by binding it*/
	glBindBuffer(GL_ARRAY_BUFFER, vboFrustum);
	/*Copy the points into the current buffer*/
	glBufferData(GL_ARRAY_BUFFER, 2*sizeof(planePositions), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(planePositions), planePositions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(planePositions), sizeof(planeColors), planeColors);

    glUseProgram(shaderProgram);
	frusPosition.push_back(pP);
	frusColor.push_back(pC);
	frusPosition[2] = glGetAttribLocation(shaderProgram, "vPosition");
	glEnableVertexAttribArray(frusPosition[2]);
	glVertexAttribPointer(frusPosition[2], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	frusColor[2] = glGetAttribLocation(shaderProgram, "vColor");
	glEnableVertexAttribArray(frusColor[2]);
	glVertexAttribPointer(frusColor[2], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(planePositions)));
	uModelViewMatrixModel = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
	uCsMatrixModel = glGetUniformLocation(shaderProgram, "csMatrix");
	uDcsMatrixModel = glGetUniformLocation(shaderProgram, "dcs");
	uNdcsFlagModel = glGetUniformLocation(shaderProgram, "NDCS");

}

void initMatrices(){
    glm::vec3 eye3 = glm::vec3(eyeCord[0], eyeCord[1], eyeCord[2]);
    glm::vec3 L3 = glm::vec3(LCord[0], LCord[1], LCord[2]);
    glm::vec3 U3 = glm::vec3(UCord[0], UCord[1], UCord[2]);
    glm::vec3 nVec = glm::normalize(eye3 - L3);
    glm::vec3 uVec = glm::normalize(glm::cross(U3, nVec));
    glm::vec3 vVec = glm::cross(nVec, uVec);
    float vcsToWcsArr[16] = {uVec[0], vVec[0], nVec[0], eye3[0],
                        uVec[1], vVec[1], nVec[1], eye3[1],
                        uVec[2], vVec[2], nVec[2], eye3[2],
                        0, 0, 0, 1
                        };
    vcsToWcs = glm::transpose(glm::make_mat4(vcsToWcsArr));
    float wcsToVcsArr[16] = {uVec[0], vVec[0], nVec[0], 0.0,
                        uVec[1], vVec[1], nVec[1], 0.0,
                        uVec[2], vVec[2], nVec[2], 0.0,
                        0, 0, 0, 1
                        };
    wcsToVcs = glm::make_mat4(wcsToVcsArr);
    glm::vec4 ePrime = wcsToVcs * eyeCord;
    wcsToVcs[3][0] = -1*ePrime[0];
    wcsToVcs[3][1] = -1*ePrime[1];
    wcsToVcs[3][2] = -1*ePrime[2];
    float L = -1*lrtb[0];
    float R = lrtb[1];
    float T = lrtb[2];
    float B = -1*lrtb[3];
    float vcsToCcsArr[16] = {2*near/(R - L), 0, 0, 0,
                            0, 2*near/(T - B), 0, 0,
                            (R + L)/(R - L), (T + B)/(T - B), -1*(far + near)/(far - near), -1.0,
                            0, 0, -2*far*near/(far - near), 0};
    vcsToCcs = glm::make_mat4(vcsToCcsArr);
    float vcsToNdcsArr[16] = {2.0/(R - L), 0, 0, 0,
                            0, 2.0/(T - B), 0, 0,
                            0, 0, -2.0/(far - near), 0,
                            (R + L)/(R - L), (T + B)/(T - B), (far + near)/(far - near), 1.0};
    vcsToNdcs = glm::make_mat4(vcsToNdcsArr);
    float ndcsToDcsArr[16] = {(Rw - Lw)/2.0, 0, 0, ((Rw-Lw)/2 + Lw),
                            0, (Tw - Bw)/2.0, 0, ((Tw-Bw)/2 + Bw),
                            0, 0, 1.0/2.0, 1.0/2.0,
                            0, 0, 0, 1};
    ndcsToDcs = glm::transpose(glm::make_mat4(ndcsToDcsArr));
}

void loadModels()
{
	std::string vertex_shader_file("vshaderModel.glsl");
    std::string fragment_shader_file("fshaderModel.glsl");

    std::vector<GLuint> shaderList;
	shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
	shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));
	shaderProgram = csX75::CreateProgramGL(shaderList);
	glUseProgram(shaderProgram);

	for (int iter = 0; iter < models.size(); iter++)
    {
        GLuint vbo;
        GLuint vao;
        vaoVec.push_back(vao);
        vboVec.push_back(vbo);
        glGenVertexArrays(1, &vaoVec[iter]);
        glBindVertexArray(vaoVec[iter]);
        glGenBuffers(1, &vboVec[iter]);
        glBindBuffer(GL_ARRAY_BUFFER, vboVec[iter]);
        // reading the model file and filling the vbo
        int len = readModel(iter);
        GLuint vP, vC;
		vPosition.push_back(vP);
		vColor.push_back(vC);
		vPosition[iter] = glGetAttribLocation(shaderProgram, "vPosition");
		glEnableVertexAttribArray(vPosition[iter]);
		glVertexAttribPointer(vPosition[iter], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		vColor[iter] = glGetAttribLocation(shaderProgram, "vColor");
		glEnableVertexAttribArray(vColor[iter]);
		glVertexAttribPointer(vColor[iter], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(len*sizeof(v_positions)));
		uModelViewMatrixModel = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
		uCsMatrixModel = glGetUniformLocation(shaderProgram, "csMatrix");

        modelRotMatrix = glm::rotate(glm::mat4(1.0f), models[iter].rot[0], glm::vec3(1.0f, 0.0f, 0.0f));
        modelRotMatrix = glm::rotate(modelRotMatrix, models[iter].rot[1], glm::vec3(0.0f, 1.0f, 0.0f));
        modelRotMatrix = glm::rotate(modelRotMatrix, models[iter].rot[2], glm::vec3(0.0f, 0.0f, 1.0f));
        modelScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(models[iter].scale[0], models[iter].scale[1], models[iter].scale[2]));
        modelTrMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(models[iter].tr[0], models[iter].tr[1], models[iter].tr[2]));
        models[iter].transformMatrix = modelTrMatrix*modelRotMatrix*modelScaleMatrix;
    }
}

void renderGL(void){
    if(mode0){
        state = 0;
        std::cout<<"\nMode set to 0";
        csMatrix = glm::mat4(1.0f);
        mode0 = false;
        ndcsBool = 0;
        dcsMatrix = glm::mat4(1.0f);
        orthoMatrix = orthoMatrix1;
        translationMatrix = translationMatrix1;
    }
    if(mode1){
        state = 1;
        std::cout<<"\nMode set to 1";
        csMatrix = wcsToVcs;
        mode1 = false;
        ndcsBool = 0;
        dcsMatrix = glm::mat4(1.0f);
        orthoMatrix = orthoMatrix1;
        translationMatrix = translationMatrix1;
    }
    if(mode2){
        state = 2;
        std::cout<<"\nMode set to 2";
        csMatrix = vcsToCcs*wcsToVcs;
        mode2 = false;
        ndcsBool = 0;
        dcsMatrix = glm::mat4(1.0f);
        orthoMatrix = orthoMatrix2;
        translationMatrix = translationMatrix2;
    }
    if(mode3){
        state = 3;
        std::cout<<"\nMode set to 3";
        csMatrix = vcsToCcs*wcsToVcs;
        mode3 = false;
        ndcsBool = 1;
        dcsMatrix = glm::mat4(1.0f);
        orthoMatrix = orthoMatrix2;
        translationMatrix = translationMatrix2;
    }
    if(mode4){
        state = 4;
        std::cout<<"\nMode set to 4";
        csMatrix = vcsToCcs*wcsToVcs;
        mode4 = false;
        ndcsBool = 1;
        dcsMatrix = ndcsToDcs;
        orthoMatrix = orthoMatrix2;
        translationMatrix = translationMatrix2;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rotationMatrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, yrot, glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, zrot, glm::vec3(0.0f, 0.0f, 1.0f));
    modelRotMatrix = orthoMatrix * rotationMatrix * translationMatrix;

    /* Render the grid if flag enabled */
    if(gridVisible){
		/* Use shader and vao/vbo for grid */
	    glUseProgram(shaderGrid);
	    glBindVertexArray(vaoBox);
	    glBindBuffer(GL_ARRAY_BUFFER, vboBox);
	    glUniformMatrix4fv(uModelViewMatrixGrid, 1, GL_FALSE, glm::value_ptr(modelRotMatrix));
        glUniformMatrix4fv(uCsMatrixGrid, 1, GL_FALSE, glm::value_ptr(csMatrix));
        glUniformMatrix4fv(uDcsMatrixGrid, 1, GL_FALSE, glm::value_ptr(dcsMatrix));
        glUniform1i(uNdcsFlagGrid, ndcsBool);
	    /* Draw */
	    glDrawArrays(GL_LINES, 0, numVertices);
    }
    glUseProgram(shaderProgram);

    glBindVertexArray(vaoLines);
    modelViewMatrix = modelRotMatrix;
    tempMatrix = csMatrix * vcsToWcs;
    glBindBuffer(GL_ARRAY_BUFFER, vboLines);
    glUniformMatrix4fv(uModelViewMatrixModel, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(uCsMatrixModel, 1, GL_FALSE, glm::value_ptr(tempMatrix));
    glUniformMatrix4fv(uDcsMatrixModel, 1, GL_FALSE, glm::value_ptr(dcsMatrix));
    glUniform1i(uNdcsFlagModel, ndcsBool);
    glDrawArrays(GL_LINES, 0, 8);

    glBindVertexArray(vaoEye);
    modelViewMatrix = modelRotMatrix;
    glBindBuffer(GL_ARRAY_BUFFER, vboEye);
    glUniformMatrix4fv(uModelViewMatrixModel, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(uCsMatrixModel, 1, GL_FALSE, glm::value_ptr(csMatrix));
    glUniformMatrix4fv(uDcsMatrixModel, 1, GL_FALSE, glm::value_ptr(dcsMatrix));
    glUniform1i(uNdcsFlagModel, ndcsBool);
    glDrawArrays(GL_POINTS, 0, 1);

    if(state != 2 and state != 3 and state != 4){
        glBindVertexArray(vaoFrustum);
        modelViewMatrix = modelRotMatrix;
        tempMatrix = csMatrix*vcsToWcs;
        glBindBuffer(GL_ARRAY_BUFFER, vboFrustum);
        glUniformMatrix4fv(uModelViewMatrixModel, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(uCsMatrixModel, 1, GL_FALSE, glm::value_ptr(tempMatrix));
        glUniformMatrix4fv(uDcsMatrixModel, 1, GL_FALSE, glm::value_ptr(dcsMatrix));
        glUniform1i(uNdcsFlagModel, ndcsBool);
        glDrawArrays(GL_TRIANGLES, 0, 12);
    }

    for(int iter=0; iter<models.size(); iter++){

        glBindVertexArray(vaoVec[iter]);
        modelViewMatrix = modelRotMatrix;
        tempMatrix = csMatrix * models[iter].transformMatrix;
        glBindBuffer(GL_ARRAY_BUFFER, vboVec[iter]);
        glUniformMatrix4fv(uModelViewMatrixModel, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
        glUniformMatrix4fv(uCsMatrixModel, 1, GL_FALSE, glm::value_ptr(tempMatrix));
        glUniformMatrix4fv(uDcsMatrixModel, 1, GL_FALSE, glm::value_ptr(dcsMatrix));
        glUniform1i(uNdcsFlagModel, ndcsBool);
        glDrawArrays(GL_TRIANGLES, 0, models[iter].vertexCount);
    }
}

int main(int argc, char **argv)
{

    std::string fileName = "../data/myscene.scn";
    readFile(fileName);
    initMatrices();
    std::cout<<"/nThe cords: "<<eyeCord[0]<<" "<<eyeCord[1]<<" "<<eyeCord[2]<<std::endl;
    std::cout<<"/n near far : "<<near <<" "<<far;
    //! The pointer to the GLFW window
    GLFWwindow *window;

    //! Setting up the GLFW Error callback
    glfwSetErrorCallback(csX75::error_callback);

    //! Initialize GLFW
    if (!glfwInit())
        return -1;

    //We want OpenGL 4.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //This is for MacOSX - can be omitted otherwise
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //We don't want the old OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //! Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(512, 512, "CS475/CS675 Tutorial 3: Rotating  Colorcube", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //! Make the window's context current
    glfwMakeContextCurrent(window);

    //Initialize GLEW
    //Turn this on to get Shader based OpenGL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        //Problem: glewInit failed, something is seriously wrong.
        std::cerr << "GLEW Init Failed : %s" << std::endl;
    }

    //Print and see what context got enabled
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    //Keyboard Callback
    glfwSetKeyCallback(window, csX75::key_callback);
    //Framebuffer resize callback
    glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


  //Initialize GL state
  csX75::initGL();
//  initBuffersGL();
    initGrid();
    loadModels();
    initFrustum();

    csMatrix = glm::mat4(1.0f);

  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
    {

      // Render here
      renderGL();

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
    }

  glfwTerminate();
    return 0;
}

//-------------------------------------------------------------------------
