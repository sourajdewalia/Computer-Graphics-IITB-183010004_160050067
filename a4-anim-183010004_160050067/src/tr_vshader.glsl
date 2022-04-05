#version 330

in vec4 vPosition;

out vec4 color;

uniform mat4 uModelViewMatrix;
//uniform mat3 normalMatrix;
//uniform mat4 viewMatrix;
uniform vec4 colorVar;

void main (void) 
{
  gl_PointSize = 10;
  gl_Position = uModelViewMatrix * vPosition;
  
  color = colorVar;
}
