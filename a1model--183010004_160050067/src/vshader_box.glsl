#version 330

in vec4 vPosition;
out vec4 color;

uniform mat4 uTranslationMatrix;
uniform mat4 uModelViewMatrix;
uniform vec4 color_in;

void main () 
{
  gl_Position = uModelViewMatrix*uTranslationMatrix*vPosition;
  color = color_in;
}
