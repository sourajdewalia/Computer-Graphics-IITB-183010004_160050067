#version 330


out vec4 frag_color;

uniform samplerCube skybox;
//in vec2 tex;
in vec3 tex;
void main () 
{
      frag_color = texture(skybox, tex);		
}
