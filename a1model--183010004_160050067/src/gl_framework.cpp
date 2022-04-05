#include "gl_framework.hpp"

extern GLint  c_offset_x,
              c_offset_y,
              c_offset_z;
extern GLfloat xrot, yrot, zrot;
extern bool saveCube,   deleteCube,
            changeColor,gridVisible,
            modelReset, inspMode,
            saveModel,  readModel;

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
  else if (key == GLFW_KEY_I && action == GLFW_PRESS){
    inspMode = true;
    std::cout << "\nInspection mode is activated.\n";
  }
  else if (key == GLFW_KEY_M && action == GLFW_PRESS){
    inspMode = false;
    std::cout << "\nModelling mode is activated.\n";
  }
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

  /* Keys only in modeling mode */
  if (!inspMode)
  {
    if (key == GLFW_KEY_X && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT)
      c_offset_x += 1;
    else if (key == GLFW_KEY_X && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
      c_offset_x -= 1;
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT)
      c_offset_y += 1;
    else if (key == GLFW_KEY_Y && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
      c_offset_y -= 1;
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT)
      c_offset_z += 1;
    else if (key == GLFW_KEY_Z && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
      c_offset_z -= 1;
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
      saveCube = true;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
      deleteCube = true;
    else if (key == GLFW_KEY_C && action == GLFW_PRESS)
      changeColor = true;
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
      modelReset = true;
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
      saveModel = true;
    else if (key == GLFW_KEY_K && action == GLFW_PRESS)
      readModel = true;
  }
}
}; // namespace csX75
