#include "trajectory_framework.hpp"

extern GLfloat mouseX, mouseY;
extern bool mouseLeftPress, mouseRightPress, deletePoint, savePoints, loadPoints;
extern GLfloat c_xrot, c_yrot, c_zrot;
extern GLfloat c_xpos, c_ypos, c_zpos;

namespace csX75
{
//! Initialize GL State
void initGL(void)
{
  //Set framebuffer clear color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //Set depth buffer furthest depth
  glClearDepth(1.0);
  //Set depth test to less-than
  glDepthFunc(GL_LESS);
  //Enable depth testing
  glEnable(GL_DEPTH_TEST);
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
  else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    deletePoint = true;
  else if (key == GLFW_KEY_T && action == GLFW_PRESS)
    savePoints = true;
  else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    loadPoints = true;
  else if (key == GLFW_KEY_LEFT)
  {
    c_xpos -= 0.10;
  }
  else if (key == GLFW_KEY_RIGHT)
  {
    c_xpos += 0.1;
  }
  else if (key == GLFW_KEY_UP)
  {
    c_ypos += 0.1;
  }
  else if (key == GLFW_KEY_DOWN)
  {
    c_ypos -= 0.1;
  }
  else if (key == GLFW_KEY_LEFT_BRACKET)
  {
    c_zpos -= 0.1;
  }
  else if (key == GLFW_KEY_RIGHT_BRACKET)
  {
    c_zpos += 0.1;
  }
  //    else if(key == GLFW_KEY_P && action == GLFW_PRESS)
  //      wireframe=!wireframe;
  else if (key == GLFW_KEY_A)
    c_yrot -= 1.0;
  else if (key == GLFW_KEY_D)
    c_yrot += 1.0;
  else if (key == GLFW_KEY_W)
    c_xrot -= 1.0;
  else if (key == GLFW_KEY_S)
    c_xrot += 1.0;
  else if (key == GLFW_KEY_Q)
    c_zrot -= 1.0;
  else if (key == GLFW_KEY_E)
    c_zrot += 1.0;
}

void mouse_button_callback(GLFWwindow *window, int key, int action, int mods)
{
  if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    mouseLeftPress = true;
  }
  else if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
  {
    mouseRightPress = true;
  }
}

void mouse_callback(GLFWwindow *window, double x, double y)
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  //std::cout<<x<<" "<<y<<std::endl;
  mouseX = x / double(width);
  mouseY = y / double(height);
}
}; // namespace csX75
