#include "gl_framework.hpp"

extern GLfloat xrot, yrot, zrot, c_xrot, c_yrot, c_zrot, earth_rot;
extern GLfloat c_xpos, c_ypos, c_zpos;
extern int tesselation;
extern bool enable_perspective, launch;
//,wireframe;
float CRange = 10000.0;
namespace csX75
{
//! Initialize GL State
void initGL(void)
{
  //Set framebuffer clear color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  //Set depth buffer furthest depth
  glClearDepth(1.0);
  //Set depth test to less-than
  glDepthFunc(GL_LESS);
  //Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
}

//!GLFW Error Callback
void error_callback(int error, const char *description)
{
  std::cerr << description << std::endl;
}

//!GLFW framebuffer resize callback
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  //!Resize the viewport to fit the window size - draw to entire window
  glViewport(0, 0, width, height);
}

//!GLFW keyboard callback
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  //!Close the window if the ESC key was pressed
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_LEFT)
  {
    if (c_xpos > -CRange)
      c_xpos -= 0.10;
  }
  else if (key == GLFW_KEY_RIGHT)
  {
    if (c_xpos < CRange)
      c_xpos += 0.1;
  }
  else if (key == GLFW_KEY_UP)
  {
    if (c_ypos < CRange)
      c_ypos += 0.1;
  }
  else if (key == GLFW_KEY_DOWN)
  {
    if (c_ypos > -CRange)
      c_ypos -= 0.1;
  }
  else if (key == GLFW_KEY_LEFT_BRACKET)
  {
    if (c_zpos > -CRange)
      c_zpos -= 0.1;
  }
  else if (key == GLFW_KEY_RIGHT_BRACKET)
  {
    if (c_zpos < CRange)
      c_zpos += 0.1;
  }
  //    else if(key == GLFW_KEY_P && action == GLFW_PRESS)
  //      wireframe=!wireframe;
  else if (key == GLFW_KEY_F)
    c_yrot -= 1.0;
  else if (key == GLFW_KEY_H)
    c_yrot += 1.0;
  else if (key == GLFW_KEY_T)
    c_xrot -= 1.0;
  else if (key == GLFW_KEY_G)
    c_xrot += 1.0;
  else if (key == GLFW_KEY_R)
    c_zrot -= 1.0;
  else if (key == GLFW_KEY_Y)
    c_zrot += 1.0;
  else if (key == GLFW_KEY_L)
  {
    launch = true;
    glfwSetTime(0);
  }
  else if (key == GLFW_KEY_A)
    earth_rot += 1.0;
  else if (key == GLFW_KEY_D)
    earth_rot -= 1.0;
}
}; // namespace csX75
