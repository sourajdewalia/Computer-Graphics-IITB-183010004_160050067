#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

glm::mat4 ipolateRotMat(glm::mat4 a1, glm::mat4 a2, float t);
float ipolateFloat(float a, float b, float t);

#endif
