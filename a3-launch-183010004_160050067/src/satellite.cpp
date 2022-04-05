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

#include "satellite.hpp"
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
ObjModel *obox, *olwing, *orwing;

//-----------------------------------------------------------------

void initBuffersGL(void)
{

  // Load shaders and use the resulting shader program
  obox = new ObjModel("../images/satBox.obj");
  olwing = new ObjModel("../images/solarWingL.obj");
  orwing = new ObjModel("../images/solarWing.obj");

  box = new HNode(NULL, obox);
  w1l = new HNode(box, olwing);
  w1l->change_parameters(-20, -20, 20, 0, 0, 0);
  w2l = new HNode(w1l, olwing);
  w2l->change_parameters(-40, 0, 0, 0, 0, 0);
  w3l = new HNode(w2l, olwing);
  w3l->change_parameters(-40, 0, 0, 0, 0, 0);
  w1r = new HNode(box, orwing);
  w1r->change_parameters(20, -20, 20, 0, 0, 0);
  w2r = new HNode(w1r, orwing);
  w2r->change_parameters(40, 0, 0, 0, 0, 0);
  w3r = new HNode(w2r, orwing);
  w3r->change_parameters(40, 0, 0, 0, 0, 0);
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

  box->render_tree(view_matrix, glm::mat4(1.0f), normal_matrix);
  //cubePtr->render(view_matrix, modelview_matrix, normal_matrix);
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
