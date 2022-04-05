#include "gl_framework.hpp"

extern GLfloat xrot,yrot,zrot;
extern bool gridVisible, mode0,
            mode1, mode2, mode3, mode4;

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
  }
  
  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }
  
  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
  }
  
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
      yrot -= glm::pi<float>() / 6.0;
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
      yrot += glm::pi<float>() / 6.0;
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
      xrot += glm::pi<float>() / 6.0;
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
      xrot -= glm::pi<float>() / 6.0;
    else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
      zrot += glm::pi<float>() / 6.0;
    else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
      zrot -= glm::pi<float>() / 6.0;
    else if (key == GLFW_KEY_H && action == GLFW_PRESS)
        gridVisible = !gridVisible;
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        mode0 = true;
    else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        mode1 = true;
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        mode2 = true;
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        mode3 = true;
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        mode4 = true;
  }
};  
  


