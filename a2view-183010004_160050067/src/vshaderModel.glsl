#version 330

in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform mat4 uModelViewMatrix;
uniform mat4 csMatrix;
uniform int NDCS;
uniform mat4 dcs;
varying vec4 temp;

void main ()
{
  gl_PointSize = 5;
  if(NDCS == 1){
    temp = csMatrix*vPosition;
    temp = temp/temp[3];
    gl_Position = uModelViewMatrix*dcs*temp;
  }
  else
    gl_Position = uModelViewMatrix*csMatrix*vPosition;
  color = vColor;
}
