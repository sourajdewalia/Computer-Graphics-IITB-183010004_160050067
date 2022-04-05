/*
  CSX75 Tutorial 5(a) Gouraud Shading

  A program which draws a tesselated sphere 
  and applies gouraud shading to it.
  -------------------------------------
  To run the code
  ./05_shading <amount_of_tesselation>
  -------------------------------------
  
  as long as amount of tesselation is between(20 & 360)
   
  Use the arrow keys and PgUp,PgDn,
  keys to make the sphere move.
  
  Use the keys W, A, S, D to move Camera.

  At starting the scene is in Perspective Mode, 
  pressing P toggles the Wireframe.

  Pressing G toggles different shading methods.

  Modified from An Introduction to OpenGL Programming, 
  Ed Angel and Dave Shreiner, SIGGRAPH 2013

  Written by Aditya Prakash, 2015
*/

#include "rocket.hpp"
#include "texture.hpp"
#include "obj_model.hpp"
#include "hierarchy_node.hpp"

int tesselation = 50;

ObjModel *cubePtr;

double PI = 3.14159265;

glm::mat4 rotation_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;

glm::mat4 model_matrix;
glm::mat4 view_matrix;

glm::mat4 modelview_matrix;
glm::mat3 normal_matrix;

GLuint uModelViewMatrix;
GLuint viewMatrix;
GLuint normalMatrix;
float disp = 0;
float step = 0.001;
double timer = 0.0;
//-----------------------------------------------------------------

const int num_vertices = 100000;

int tri_idx = 0;
glm::vec4 v_positions[num_vertices];
glm::vec4 v_colors[num_vertices];
glm::vec4 v_normals[num_vertices];
glm::vec2 tex_coords[num_vertices];

glm::vec4 color(0.6, 0.6, 0.6, 1.0);
glm::vec4 black(0.2, 0.2, 0.2, 1.0);
glm::vec4 white(1.0, 1.0, 1.0, 1.0);

//csX75::HNode *box, *w1l, *w2l, *w3l, *w1r, *w2r, *w3r;
ObjModel *mainBoost, *mainTop, *sideBoost, *sideTop;
ObjModel *mainThrust, *sideThrust;
ObjModel *launchSite;
//-----------------------------------------------------------------

void initBuffersGL(void)
{

  // Load shaders and use the resulting shader program
  mainBoost = new ObjModel("../images/mainBooster.obj");
  mainTop = new ObjModel("../images/mainTop.obj");
  sideBoost = new ObjModel("../images/auxBooster.obj");
  sideTop = new ObjModel("../images/sideTop.obj");
  mainThrust = new ObjModel("../images/mainExh.obj");
  sideThrust = new ObjModel("../images/sideExh.obj");
  launchSite = new ObjModel("../images/cube.obj");

  launchSiteNode = new HNode(NULL, launchSite);
  launchSiteNode->change_parameters(0, 0, 0, -90, 0, 0);
  mainBoostNode = new HNode(launchSiteNode, mainBoost);
  mainBoostNode->change_parameters(0, 0, 0, 90, 0, 0); disp = 0;
  mainTopNode = new HNode(mainBoostNode, mainTop);
  mainTopNode->change_parameters(0, 10, 0, 0, 0, 0);
  aBoostLNode = new HNode(mainBoostNode, sideBoost);
  aBoostLNode->change_parameters(-1.8, -2.5, 0, 0, 0, 0);
  aBoostRNode = new HNode(mainBoostNode, sideBoost);
  aBoostRNode->change_parameters(1.8, -2.5, 0, 0, 0, 0);
  aTopLNode = new HNode(aBoostLNode, sideTop);
  aTopLNode->change_parameters(0, 7.5, 0, 0, 0, 0);
  aTopRNode = new HNode(aBoostRNode, sideTop);
  aTopRNode->change_parameters(0, 7.5, 0, 0, 0, 0);
  mainThrustNode = new HNode(mainBoostNode, mainThrust);
  mainThrustNode->change_parameters(0, -11.5, 0, 0, 0, 0);
  LThrustNode = new HNode(aBoostLNode, sideThrust);
  LThrustNode->change_parameters(0, -9, 0, 0, 0, 0);
  RThrustNode = new HNode(aBoostRNode, sideThrust);
  RThrustNode->change_parameters(0, -9, 0, 0, 0, 0);
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  matrixStack.clear();

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f, 0.0f, 0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f, 1.0f, 0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f, 0.0f, 1.0f));
  model_matrix = rotation_matrix;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f, 0.0f, 0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f, 1.0f, 0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos, c_ypos, c_zpos, 1.0);
  glm::vec4 c_up = glm::vec4(c_up_x, c_up_y, c_up_z, 1.0) * c_rotation_matrix;
  glm::vec4 c_lookat = glm::vec4(0.0, 0.0, -1.0, 1.0) * c_rotation_matrix;
  c_lookat = c_lookat + c_pos;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos), glm::vec3(c_lookat), glm::vec3(c_up));

  //creating the projection matrix

  projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 2.0, 500.0);

  view_matrix = projection_matrix * lookat_matrix;

  matrixStack.push_back(view_matrix);

  modelview_matrix = view_matrix * model_matrix;
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));

  if(launch)
  {
    timer = glfwGetTime();
    if (timer < 3)
      disp += 0.01*timer;
    else
      disp += 0.03 * timer;
    mainBoostNode->change_parameters(0, 0, disp, 90, 0, 0);
    if (disp>100)
      launch = false;
  }
  //  mainBoostNode->render_tree(view_matrix, glm::mat4(1.0f), normal_matrix);
  //cubePtr->render(view_matrix, modelview_matrix, normal_matrix);
  launchSiteNode->render_tree(view_matrix, glm::mat4(1.0f), normal_matrix);
}

int main(int argc, char **argv)
{
  //! The pointer to the GLFW window
  GLFWwindow *window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;
  if (argc > 1)
    tesselation = atoi(argv[1]);
  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(512, 512, "CS475/CS675 Tutorial 5: Shading a Sphere", NULL, NULL);
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
  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  //Initialize GL state
  csX75::initGL();
  initBuffersGL();

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
