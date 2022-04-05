/*
  A program which opens a window and draws the "color cube."

  Use the arrow keys and PgUp,PgDn, 
  keys to make the cube move.

  Written by - 
               Parag Chaudhuri
*/
#ifndef _COLORCUBE_HPP_
#define _COLORCUBE_HPP_

// Defining the ESCAPE Key Code
#define ESCAPE 27
// Defining the DELETE Key Code
#define DELETE 127

#include "trajectory_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

// Translation Parameters
GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 1.0;
// Rotation Parameters
GLfloat c_xrot = 0.0, c_yrot = 0.0, c_zrot = 0.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat mouseX = 0.0, mouseY = 0.0, mouseZ = 0.0;
bool mouseLeftPress = false;
bool mouseRightPress = false;
bool deletePoint = false;
bool savePoints = false;
bool loadPoints = false;

int mode = 0; // mode is 0 for drawing points and 1 for editing the points

//Running variable to toggle culling on/off
bool enable_culling = true;
//Running variable to toggle wireframe/solid modelling
bool solid = true;

//-------------------------------------------------------------------------

#endif
