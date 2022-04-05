#version 330

//in vec4 color;
out vec4 frag_color;

uniform sampler2D texture;
varying vec2 tex;
void main () 
{
  //frag_color = color;
  frag_color = texture2D(texture, tex);
}
