#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_
GLuint LoadTexture( const char * filename);
GLuint LoadTextureAdv( const char * filename);
void FreeTexture( GLuint texture );
GLuint LoadCubeMap(std::vector<std::string> faces);
#endif 
