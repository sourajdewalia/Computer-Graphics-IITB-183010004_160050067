/*
  A program which opens a window and draws the "color cube."

  Use the arrow keys and PgUp,PgDn, 
  keys to make the cube move.

  Written by - 
               Parag Chaudhuri
*/
#ifndef _ROCKET_HPP_
#define _ROCKET_HPP_

// Defining the ESCAPE Key Code
#define ESCAPE 27
// Defining the DELETE Key Code
#define DELETE 127

#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "hierarchy_node.hpp"
#include "keyframe.hpp"
#include "util.hpp"
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include <thread>
#include<chrono>

// Translation Parameters
GLfloat xpos = 0.0, ypos = 0.0, zpos = 0.0;
// Rotation Parameters
GLfloat xrot = 0.0, yrot = 0.0, zrot = 0.0;
// Camera position and rotation Parameters
GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 5.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot = 0.0, c_yrot = 0.0, c_zrot = 0.0;
GLfloat earth_rot = 0.0;
//Running variable to toggle culling on/off
bool enable_culling = true;
bool launch = false;
bool mainLanding, auxLanding;
//Running variable to toggle wireframe/solid modelling
bool solid = true;
//Enable/Disable perspective view
bool enable_perspective = false;
std::vector<glm::mat4> matrixStack;
HNode *mainBoostNode, *mainTopNodeL, *mainTopNodeR, *aBoostLNode, *aBoostRNode, *aTopLNode, *aTopRNode,
		*payloadBayNodeL, *payloadBayNodeR, *ringNode;
HNode *mainThrustNode, *LThrustNode, *RThrustNode;
HNode *launchSiteNode, *standNode, *towerNode, *launchpadNode, *harnessNode, *earthNode;
std::vector<HNode *> mainStandNodes, aStandNodesL, aStandNodesR;
//-------------------------------------------------------------------------
#endif
