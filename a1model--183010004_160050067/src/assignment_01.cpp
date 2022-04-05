/*
  CS675: Assignment 1

  A program to make a (cubified) model of a structure/building.

  Format/Boilerplate code taken from the tutorial "02_colorcube" of
  course CS675/475 by Prof. Parag Chaudhary.
  
*/

#include "assignment_01.hpp"

/* Constants defining grid specification*/
const int   gridSize = 50;
const float edge = 1.0f / (gridSize - 1);
const int   numVertices = (gridSize - 1) * (gridSize) * (gridSize)*6;

/*---------------------------------------------------------------*/
/* Shaders and VBOs/VAOs*/
GLuint  shaderProgram,
        shaderProgramBox;
GLuint  vbo, vao,
        vboBox, vaoBox;

/* Matrices for transformations */
glm::mat4 orthoMatrix = glm::ortho(-1.2, 1.2, -1.2, 1.2, -1.2, 1.2),
          translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f)),
          translationMatrixCursor,
          translationMatrixBoxes,
          rotationMatrix,
          modelViewMatrix;

/* Uniform data to be passed to shader */
GLuint  uModelViewMatrixGrid,
        uModelViewMatrixModel,
        uTrMatrix,
        uBoxColor;

/* Vector to store current box colors */
glm::vec4 boxColor,
          cursorColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), /* Green color for cursor */
          activeColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); /* Currently active color */

/* Grid vertices array */
glm::vec4 positions[gridSize][gridSize][gridSize],
          v_positions[(gridSize - 1) * (gridSize) * (gridSize)*6];

/* Arrays for storing persistent model data */
bool cubesPresent[gridSize][gridSize][gridSize];
glm::vec4 userColors[gridSize][gridSize][gridSize];

/*---------------------------------------------------------------*/
/* Function to generate grid and initialise storage arrays */
void grid(void)
{
  for (int i = 0; i < gridSize; i++)
    for (int j = 0; j < gridSize; j++)
      for (int k = 0; k < gridSize; k++)
      {
        positions[i][j][k] = glm::vec4((i * edge), (j * edge), (k * edge), 1.0);
        cubesPresent[i][j][k] = false;
        userColors[i][j][k] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
      }
}

/* Generate an array of coupled points for grid generation */
void generateGrid(void)
{
  int idx = 0;
  for (int i = 0; i < gridSize; i++)
    for (int j = 0; j < gridSize; j++)
      for (int k = 0; k < gridSize; k++){
        if (i + 1 < gridSize){
          v_positions[idx++] = positions[i][j][k];
          v_positions[idx++] = positions[i + 1][j][k];
        }
        if (j + 1 < gridSize){
          v_positions[idx++] = positions[i][j][k];
          v_positions[idx++] = positions[i][j + 1][k];
        }
        if (k + 1 < gridSize){
          v_positions[idx++] = positions[i][j][k];
          v_positions[idx++] = positions[i][j][k + 1];
        }
      }
}

/* Functions to create a box at 0,0,0 */
int tri_idx = 0;
const int numVerticesBox = 36;
glm::vec4 vPositionsBox[numVerticesBox];
glm::vec4 positionsBox[8];

void createBox(int i, int j, int k)
{
  positionsBox[0] = positions[i][j][k + 1];
  positionsBox[1] = positions[i][j + 1][k + 1];
  positionsBox[2] = positions[i + 1][j + 1][k + 1];
  positionsBox[3] = positions[i + 1][j][k + 1];
  positionsBox[4] = positions[i][j][k];
  positionsBox[5] = positions[i][j + 1][k];
  positionsBox[6] = positions[i + 1][j + 1][k];
  positionsBox[7] = positions[i + 1][j][k];
}

/*quad generates two triangles for each face and assigns colors to the vertices*/
void quad(int a, int b, int c, int d)
{
  vPositionsBox[tri_idx] = positionsBox[a];tri_idx++;
  vPositionsBox[tri_idx] = positionsBox[b];tri_idx++;
  vPositionsBox[tri_idx] = positionsBox[c];tri_idx++;
  vPositionsBox[tri_idx] = positionsBox[a];tri_idx++;
  vPositionsBox[tri_idx] = positionsBox[c];tri_idx++;
  vPositionsBox[tri_idx] = positionsBox[d];tri_idx++;
}

/* generate 12 triangles: 36 vertices */
void colorCube(void)
{
  createBox(0, 0, 0);
  quad(1, 0, 3, 2);
  quad(2, 3, 7, 6);
  quad(3, 0, 4, 7);
  quad(6, 5, 1, 2);
  quad(4, 5, 6, 7);
  quad(5, 4, 0, 1);
}

/* Initialise buffers for Grid*/
void initBuffersGL(void)
{
  /* generate grid */
  grid();
  generateGrid();

  /* Ask GL for a Vertex Attribute Object (vao)*/ 
  glGenVertexArrays(1, &vao);
  /* Set it as the current array to be used by binding it*/
  glBindVertexArray(vao);

  /* Ask GL for a Vertex Buffer Object (vbo)*/
  glGenBuffers(1, &vbo);
  /*Set it as the current buffer to be used by binding it*/
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  /*Copy the points into the current buffer*/
  glBufferData(GL_ARRAY_BUFFER, sizeof(v_positions), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions);

  /* Load shaders and use the resulting shader program*/
  std::string vertex_shader_file("vshader.glsl");
  std::string fragment_shader_file("fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram(shaderProgram);

  /* set up vertex arrays */
  GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  uModelViewMatrixGrid = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
}

void initBuffersGLBox(void)
{
  /* Create a colorcube at 0,0,0 */
  colorCube();

  /*Ask GL for a Vertex Attribute Object (vao)*/
  glGenVertexArrays(1, &vaoBox);
  /*Set it as the current array to be used by binding it*/
  glBindVertexArray(vaoBox);

  /*Ask GL for a Vertex Buffer Object (vbo)*/
  glGenBuffers(1, &vboBox);
  /*Set it as the current buffer to be used by binding it*/
  glBindBuffer(GL_ARRAY_BUFFER, vboBox);
  /*Copy the points into the current buffer*/
  glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionsBox), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vPositionsBox), vPositionsBox);

  /* Load shaders and use the resulting shader program*/
  std::string vertex_shader_file("vshader_box.glsl");
  std::string fragment_shader_file("fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgramBox = csX75::CreateProgramGL(shaderList);
  glUseProgram(shaderProgramBox);

  /* set up vertex arrays */
  GLuint vPosition = glGetAttribLocation(shaderProgramBox, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  uModelViewMatrixModel = glGetUniformLocation(shaderProgramBox, "uModelViewMatrix");
  uTrMatrix = glGetUniformLocation(shaderProgramBox, "uTranslationMatrix");
  uBoxColor = glGetUniformLocation(shaderProgramBox, "color_in");
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Create the transformation matrix for all */
  rotationMatrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f, 0.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, yrot, glm::vec3(0.0f, 1.0f, 0.0f));
  rotationMatrix = glm::rotate(rotationMatrix, zrot, glm::vec3(0.0f, 0.0f, 1.0f));
    
  modelViewMatrix = orthoMatrix * rotationMatrix * translationMatrix;

  /* Render the grid if flag enabled */
  if (gridVisible)
  {
    /* Use shader and vao/vbo for grid */
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(uModelViewMatrixGrid, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    /* Draw */
    glDrawArrays(GL_LINES, 0, numVertices);
  }

  /* Use shader and vao/vbo for boxes */
  glUseProgram(shaderProgramBox);
  glBindVertexArray(vaoBox);
  glBindBuffer(GL_ARRAY_BUFFER, vboBox);

  /* Render the cursor if not in inspection mode */
  if (!inspMode)
  {
    /* Restrict the cursor inside the grid */
    if (c_offset_x < 0) c_offset_x = 0;
    if (c_offset_y < 0) c_offset_y = 0;
    if (c_offset_z < 0) c_offset_z = 0;
    if (c_offset_x > gridSize - 2) c_offset_x = gridSize - 2;
    if (c_offset_y > gridSize - 2) c_offset_y = gridSize - 2;
    if (c_offset_z > gridSize - 2) c_offset_z = gridSize - 2;

    /* Translation matrix for cursor */
    translationMatrixCursor = glm::translate(glm::mat4(1.0f), glm::vec3(c_offset_x * edge, c_offset_y * edge, c_offset_z * edge));
    glUniformMatrix4fv(uModelViewMatrixModel, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(uTrMatrix, 1, GL_FALSE, glm::value_ptr(translationMatrixCursor));
    glUniform4fv(uBoxColor, 1, glm::value_ptr(cursorColor));
    /* Draw */
    glDrawArrays(GL_TRIANGLES, 0, numVerticesBox);
  }

  /* Render persistent cubes of the model */
  for (int it_x = 0; it_x < (gridSize - 1); it_x++)
    for (int it_y = 0; it_y < (gridSize - 1); it_y++)
      for (int it_z = 0; it_z < (gridSize - 1); it_z++)
        if (cubesPresent[it_x][it_y][it_z])
        {
          /* Translation matrix for each box */
          translationMatrixBoxes = glm::translate(glm::mat4(1.0f), glm::vec3(it_x * edge, it_y * edge, it_z * edge));
          glUniformMatrix4fv(uModelViewMatrixModel, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
          glUniformMatrix4fv(uTrMatrix, 1, GL_FALSE, glm::value_ptr(translationMatrixBoxes));
          /* Color for each box */
          boxColor = userColors[it_x][it_y][it_z];
          glUniform4fv(uBoxColor, 1, glm::value_ptr(boxColor));
          /* Draw */
          glDrawArrays(GL_TRIANGLES, 0, numVerticesBox);
        }
}

/* Function to write data to file */
int writeToFile(std::string filename)
{
  std::ofstream file;
  file.open(filename);
  if(!file.is_open()){
    std::cout<<"\nSorry could not write to file..!!"<<std::endl;
    return -1;
  }
  glm::vec4 colorTemp; 
  for (int it_x = 0; it_x < (gridSize - 1); it_x++)
    for (int it_y = 0; it_y < (gridSize - 1); it_y++)
      for (int it_z = 0; it_z < (gridSize - 1); it_z++)
        if(cubesPresent[it_x][it_y][it_z])
        {
          colorTemp = userColors[it_x][it_y][it_z];
          file<<float(it_x)<<" "<<float(it_y)<<" "<<float(it_z)<<" "<<colorTemp[0]<<" "<<colorTemp[1]<<" "<<colorTemp[2]<<"\n";
        }
  file.close();
  std::cout<<"\nThe model has been successfully saved..."<<std::endl;
  return 0;
}

/* Function to read data from file */
int readFile(std::string fileName)
{
  std::ifstream file;
  file.open(fileName);
  
  if(!file.is_open()){
    std::cout<<"\nInvalid file name..!!"<<std::endl;
    return -1;
  }
  /* Reset grid */
  for (int it_x = 0; it_x < (gridSize - 1); it_x++)
    for (int it_y = 0; it_y < (gridSize - 1); it_y++)
      for (int it_z = 0; it_z < (gridSize - 1); it_z++)
        cubesPresent[it_x][it_y][it_z] = false;
  
  while(!file.eof())
  {
    std::string lineIn;
    getline(file, lineIn);
    std::istringstream stream(lineIn);
    int count = 0;
    int it_x = 0,
        it_y = 0,
        it_z = 0;
    std::string s;
    while(getline(stream, s, ' ')){
      float val = std::atof(s.c_str());
      if(count == 0)      it_x = std::round(val);
      else if(count == 1) it_y = std::round(val);
      else if(count == 2) it_z = std::round(val);
      else if(count == 3){cubesPresent[it_x][it_y][it_z] = true;
                          userColors[it_x][it_y][it_z][0] = val;}
      else if(count == 4) userColors[it_x][it_y][it_z][1] = val;
      else if(count == 5) userColors[it_x][it_y][it_z][2] = val;
      count++;
    }
  }
  file.close();
  return 0;
}

/* main function */
int main(int argc, char **argv)
{
  /* The pointer to the GLFW window */
  GLFWwindow *window;

  /* Setting up the GLFW Error callback */
  glfwSetErrorCallback(csX75::error_callback);

  /* Initialize GLFW */
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(720, 720, "CS475/675 Assignment 1: Cubified Model", NULL, NULL);
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

  /* Initialize GL state and Buffers*/
  csX75::initGL();
  initBuffersGL();
  initBuffersGLBox();

  /* Loop until the user closes the window */
  while (glfwWindowShouldClose(window) == 0)
  {
    /* if the cube is marked */
    if (saveCube){
      saveCube = false;
      cubesPresent[c_offset_x][c_offset_y][c_offset_z] = true;
      userColors[c_offset_x][c_offset_y][c_offset_z]   = activeColor;
    }

    /* if drawing color is changed */
    if (changeColor){
      float red, green, blue;
      std::cout << "\nEnter the  red  component (R):";
      std::cin >> red;
      std::cout <<   "Enter the green component (G):";
      std::cin >> green;
      std::cout <<   "Enter the  blue component (B):";
      std::cin >> blue;
      activeColor = glm::vec4(red, green, blue, 1.0f);
      changeColor = false;
    }

    /* if cube is deleted*/
    if (deleteCube){
      cubesPresent[c_offset_x][c_offset_y][c_offset_z] = false;
      deleteCube = false;
    }

    /* if model is saved */
    if(saveModel){
      std::string fileName;
      std::cout<<"\nEnter the filename for the model to be written: ";
      std::cin>>fileName;
      writeToFile(fileName);
      saveModel = false;
    }

    /* if model is to be read */
    if(readModel){
      std::string fileName;
      std::cout<<"\nEnter the filename of the model to be loaded: ";
      std::cin>>fileName;
      readFile(fileName);
      readModel = false;
    }

    /* if grid is reset*/
    if (modelReset){
      for (int it_x = 0; it_x < (gridSize - 1); it_x++)
        for (int it_y = 0; it_y < (gridSize - 1); it_y++)
          for (int it_z = 0; it_z < (gridSize - 1); it_z++)
            cubesPresent[it_x][it_y][it_z] = false;
      modelReset = false;
    }

    /* Render */
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
