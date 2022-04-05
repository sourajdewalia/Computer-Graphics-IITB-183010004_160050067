/*
  A program which opens a window and draws the "color cube."

  Modified from An Introduction to OpenGL Programming, 
  Ed Angel and Dave Shreiner, SIGGRAPH 2013

  Written by Parag Chaudhuri, 2015
*/
#ifndef _COLORCUBE_HPP_
#define _COLORCUBE_HPP_

//Include Files
#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

/* offset for boxes */
GLint 	c_offset_x = 0,
		c_offset_y = 0,
		c_offset_z = 0;
/* rotational parameters */
GLfloat xrot = 0.0, yrot = 0.0, zrot = 0.0;

/* Key callback flags */
bool 	saveCube 	= false,
		deleteCube 	= false,
		changeColor = false,
		gridVisible = true,
		modelReset 	= false,
		inspMode 	= false,
		saveModel 	= false,
		readModel 	= false;

#endif
