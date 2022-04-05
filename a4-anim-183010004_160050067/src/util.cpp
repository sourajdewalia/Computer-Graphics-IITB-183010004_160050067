#include "util.hpp"

glm::mat4 ipolateRotMat(glm::mat4 a1, glm::mat4 a2, float t)
{
	glm::mat4 a;
	return a;
}

float ipolateFloat(float a, float b, float t){
	float res = a*(1-t) + b*(t);
	return res;
}