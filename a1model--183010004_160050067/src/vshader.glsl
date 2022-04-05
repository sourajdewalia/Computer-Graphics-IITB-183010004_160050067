#version 330

in vec4 vPosition;
out vec4 color;
uniform mat4 uModelViewMatrix;

void main () 
{
	gl_Position = uModelViewMatrix * vPosition;
  	color = vec4(0.9f,0.9f,0.9f,1.0f);
}
