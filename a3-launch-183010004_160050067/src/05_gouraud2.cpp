/*
  CSX75 Tutorial 5(a) Gouraud Shading

  A program which draws a tesselated sphere 
  and applies gouraud shading to it.
  -------------------------------------
  To run the code
  ./05_shading <amount_of_tesselation>
  -------------------------------------
  
  as long as amount of tesselation is between(20 & 360)
   
  Use the arrow keys and PgUp,PgDn,
  keys to make the sphere move.
  
  Use the keys W, A, S, D to move Camera.

  At starting the scene is in Perspective Mode, 
  pressing P toggles the Wireframe.

  Pressing G toggles different shading methods.

  Modified from An Introduction to OpenGL Programming, 
  Ed Angel and Dave Shreiner, SIGGRAPH 2013

  Written by Aditya Prakash, 2015
*/

#include "05_gouraud.hpp"
#include "texture.hpp"

const int nOfC = 4;

double PI = 3.14159265;
GLuint shaderProgram;
GLuint vbo[nOfC], vao[nOfC];
GLuint tex, tex2;

float scl = 0.0;
glm::mat4 rotation_matrix;
glm::mat4 scale_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;

glm::mat4 model_matrix;
glm::mat4 view_matrix;

glm::mat4 modelview_matrix;
glm::mat3 normal_matrix;

GLuint uModelViewMatrix;
GLuint viewMatrix;
GLuint normalMatrix;
//-----------------------------------------------------------------

std::vector<glm::vec4>  v_positions,
                        v_colors,
                        v_normals;

std::vector<glm::vec2>  tex_coords;

glm::vec4 color(0.6, 0.6, 0.6, 1.0);
glm::vec4 black(0.2, 0.2, 0.2, 1.0);
glm::vec4 white(1.0, 1.0, 1.0, 1.0);

double Radius = 1;
int Lat = 1000;
int Long = 1000;
float len = 20.0;
int numVertices[nOfC];

void cylinder(double radius, int Lats, int Longs, float length)
{
  float lats, longs;

  float slices= 2 * PI/Lat;
  float dz = length/Longs;

  for (lats = 0.0; lats < length; lats+=dz)  
  {
    for(longs = 0.0; longs <= 2.0*PI+slices; longs+=slices)
    {
      float x = radius * cos(-longs);
      float y = radius * sin(-longs);
      float z = lats ;
      glm::vec4 pt(x, y, z, 1.0);
	glm::vec4 nrm(x,y,0.0,1.0);

      float tx = longs / (PI);
      /*if (longs < PI)
        tx = longs / (PI);
      else
        tx = (longs/PI)-1;*/
      glm::vec2 tex_pos(0.15+(1-tx), (z / length));
      v_colors.push_back(white);
      v_positions.push_back(pt);
      v_normals.push_back(nrm);
      tex_coords.push_back(tex_pos);

      if(lats + dz > length)
        lats = length;

      z = lats + dz;
      glm::vec4 pt2(x, y, z, 1.0);
      glm::vec2 tex_pos2(0.15+(1-tx), (z / length));
 
      v_colors.push_back(white);
      v_positions.push_back(pt2);
      v_normals.push_back(nrm);
      tex_coords.push_back(tex_pos2);
    }
  }
}

void top(double radius, int Lats, int Longs, float length)
{
  float lats, longs ,r, rp1;

  float slices= 2 * PI/Lat;
  float dz = length/Longs;

  for (lats = 0.0; lats < length; lats+=dz)  
  {
    r = sqrt((length-lats)*(radius*radius/length)) ;/*radius*0.99;}*/
    rp1 = sqrt((length-lats -dz)*(radius*radius/length));
    for(longs = 0.0; longs <= PI+slices; longs+=slices)
    {
      float x = r * cos(-longs);
      float y = r * sin(-longs);
      float z = lats ;
      glm::vec4 pt(x, y, z, 1.0);

      /*float tx;
      if (longs < PI)
        tx = longs / (PI);
      else
        tx = (longs/PI)-1;*/
      float tx = longs / (PI);
      glm::vec2 tex_pos(0.15+(1-tx)*0.3, (z / length)*0.3);
      v_colors.push_back(white);
      v_positions.push_back(pt);
      v_normals.push_back(pt);
      tex_coords.push_back(tex_pos);

      if(lats + dz > length)
        lats = length;

      z = lats + dz;
      x = rp1 * cos(-longs) ;
      y = rp1 * sin(-longs) ;
      
      glm::vec4 pt2(x, y, z, 1.0);
      glm::vec2 tex_pos2(0.15+(1-tx)*0.3, (z / length)*0.3);
 
      v_colors.push_back(white);
      v_positions.push_back(pt2);
      v_normals.push_back(pt2);
      tex_coords.push_back(tex_pos2);
    }
  }
}

//-----------------------------------------------------------------

void initBuffersGL(void)
{

  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("05_vshader.glsl");
  std::string fragment_shader_file("05_fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram(shaderProgram);

  // getting the attributes from the shader program
  GLuint vPosition = glGetAttribLocation(shaderProgram, "vPosition");
  GLuint vColor = glGetAttribLocation(shaderProgram, "vColor");
  GLuint vNormal = glGetAttribLocation(shaderProgram, "vNormal");
  GLuint vTexCoords = glGetAttribLocation(shaderProgram, "texCord");
  uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
  normalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
  viewMatrix = glGetUniformLocation(shaderProgram, "viewMatrix");

  // Load Textures
  tex = LoadTexture("../images/Falcon-Heavy-closeup-SpaceX-4-1.bmp", 235, 922);
  tex2 = LoadTexture("../images/boosters_tex.bmp", 185, 152);

//    tex = LoadTexture("../images/main_booster.bmp", 227, 127);
 // tex = LoadTexture("../images/index.bmp", 427, 1147);
//    tex = LoadTexture("../images/1_earth_8k.bmp", 1024, 512);
  glBindTexture(GL_TEXTURE_2D, tex);

  //Ask GL for two Vertex Attribute Objects (vao) , one for the colorcube and one for the plane.
  glGenVertexArrays(nOfC, vao);
  //Ask GL for two Vertex Buffer Object (vbo)
  glGenBuffers(nOfC, vbo);

  //Set 0 as the current array to be used by binding it
  glBindVertexArray(vao[0]);
  //Set 0 as the current buffer to be used by binding it
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

  // Call the sphere function
  cylinder(Radius, Lat, Long, len);
  //top(Radius, Lat, Long, 3.0);
  numVertices[0] = v_positions.size();

  glBufferData (GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size() + v_normals.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, v_positions.size()*sizeof(glm::vec4), &v_positions[0] );
  glBufferSubData( GL_ARRAY_BUFFER, v_positions.size()*sizeof(glm::vec4), v_colors.size()*sizeof(glm::vec4), &v_colors[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4), tex_coords.size()*sizeof(glm::vec2), &tex_coords[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), v_normals.size()*sizeof(glm::vec4), &v_normals[0] );


  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_positions.size()*sizeof(glm::vec4)) );

  glEnableVertexAttribArray(vTexCoords);
  glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4)));

  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2)));
  // set up vertex array

  //Set 0 as the current array to be used by binding it
  glBindVertexArray(vao[1]);
  //Set 0 as the current buffer to be used by binding it
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

  v_positions.clear();
  v_colors.clear();
  v_normals.clear();
  tex_coords.clear();
  top(Radius, Lat, Long, 3.0);
   numVertices[1] = v_positions.size();
///

  glBufferData (GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size() + v_normals.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, v_positions.size()*sizeof(glm::vec4), &v_positions[0] );
  glBufferSubData( GL_ARRAY_BUFFER, v_positions.size()*sizeof(glm::vec4), v_colors.size()*sizeof(glm::vec4), &v_colors[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4), tex_coords.size()*sizeof(glm::vec2), &tex_coords[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), v_normals.size()*sizeof(glm::vec4), &v_normals[0] );

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_positions.size()*sizeof(glm::vec4)) );

  glEnableVertexAttribArray(vTexCoords);
  glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4)));

  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2)));


  ////Boosters
    // Call the sphere function
  glBindVertexArray(vao[2]);
  //Set 0 as the current buffer to be used by binding it
  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

  v_positions.clear();
  v_colors.clear();
  v_normals.clear();
  tex_coords.clear();
  cylinder(0.5, Lat, Long, len*0.75);
  //top(Radius, Lat, Long, 3.0);
  numVertices[2] = v_positions.size();

  glBindTexture(GL_TEXTURE_2D, tex2);

  glBufferData (GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size() + v_normals.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, v_positions.size()*sizeof(glm::vec4), &v_positions[0] );
  glBufferSubData( GL_ARRAY_BUFFER, v_positions.size()*sizeof(glm::vec4), v_colors.size()*sizeof(glm::vec4), &v_colors[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4), tex_coords.size()*sizeof(glm::vec2), &tex_coords[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), v_normals.size()*sizeof(glm::vec4), &v_normals[0] );


  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_positions.size()*sizeof(glm::vec4)) );

  glEnableVertexAttribArray(vTexCoords);
  glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4)));

  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2)));
  // set up vertex array

  //Set 0 as the current array to be used by binding it
  glBindVertexArray(vao[3]);
  //Set 0 as the current buffer to be used by binding it
  glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

  v_positions.clear();
  v_colors.clear();
  v_normals.clear();
  tex_coords.clear();
  top(0.5, Lat, Long, 1.5);
  numVertices[3] = v_positions.size();
///

  glBufferData (GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size() + v_normals.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, v_positions.size()*sizeof(glm::vec4), &v_positions[0] );
  glBufferSubData( GL_ARRAY_BUFFER, v_positions.size()*sizeof(glm::vec4), v_colors.size()*sizeof(glm::vec4), &v_colors[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4), tex_coords.size()*sizeof(glm::vec2), &tex_coords[0] );
  glBufferSubData( GL_ARRAY_BUFFER, (v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2), v_normals.size()*sizeof(glm::vec4), &v_normals[0] );

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_positions.size()*sizeof(glm::vec4)) );

  glEnableVertexAttribArray(vTexCoords);
  glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4)));

  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((v_positions.size() + v_colors.size())*sizeof(glm::vec4) + tex_coords.size()*sizeof(glm::vec2)));

}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(xrot), glm::vec3(1.0f, 0.0f, 0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(yrot), glm::vec3(0.0f, 1.0f, 0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, glm::radians(zrot), glm::vec3(0.0f, 0.0f, 1.0f));

  model_matrix = rotation_matrix;

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f, 0.0f, 0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f, 1.0f, 0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos+scl, c_ypos+scl, c_zpos+scl, 1.0) * c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x, c_up_y, c_up_z, 1.0) * c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos), glm::vec3(0.0), glm::vec3(c_up));

  //creating the projection matrix

  projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 30.0);
  //projection_matrix = glm::ortho(-50.0, 50.0, -50.0, 50.0, 1.0, 10.0);
  view_matrix = projection_matrix * lookat_matrix;

  glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(view_matrix));


  modelview_matrix = view_matrix * model_matrix ;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[0]);
  glBindTexture(GL_TEXTURE_2D, tex);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[0]);

  // Draw the sphere
  model_matrix = glm::translate(model_matrix, glm::vec3(0.0,0.0,len));
  modelview_matrix = view_matrix * model_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[1]);

  model_matrix = glm::rotate(model_matrix, GLfloat(PI), glm::vec3(0.0f, 0.0f, 1.0f));
  modelview_matrix = view_matrix * model_matrix ;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[1]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[1]);

  //****booster
  model_matrix = rotation_matrix;
  model_matrix = glm::translate(model_matrix, glm::vec3(1.5,0.0,0.0));
  modelview_matrix = view_matrix * model_matrix ;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[2]);
  glBindTexture(GL_TEXTURE_2D, tex2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[2]);

  // Draw the sphere
  model_matrix = glm::translate(model_matrix, glm::vec3(0.0,0.0,len*0.75));
  modelview_matrix = view_matrix * model_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[3]);

  model_matrix = rotation_matrix;
  model_matrix = glm::rotate(model_matrix, GLfloat(PI), glm::vec3(0.0f, 0.0f, 1.0f));
  model_matrix = glm::translate(model_matrix, glm::vec3(-1.5,0.0,len*0.75));
  modelview_matrix = view_matrix * model_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[3]);

  //***booster 2
  model_matrix = rotation_matrix;
  model_matrix = glm::translate(model_matrix, glm::vec3(-1.5,0.0,0.0));
  modelview_matrix = view_matrix * model_matrix ;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[2]);
  glBindTexture(GL_TEXTURE_2D, tex2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[2]);

  // Draw the sphere
  model_matrix = glm::translate(model_matrix, glm::vec3(0.0,0.0,len*0.75));
  modelview_matrix = view_matrix * model_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[3]);

  model_matrix = rotation_matrix;
  model_matrix = glm::rotate(model_matrix, GLfloat(PI), glm::vec3(0.0f, 0.0f, 1.0f));
  model_matrix = glm::translate(model_matrix, glm::vec3(1.5,0.0,len*0.75));
  modelview_matrix = view_matrix * model_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
  glUniformMatrix3fv(normalMatrix, 1, GL_FALSE, glm::value_ptr(normal_matrix));
  glBindVertexArray(vao[3]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices[3]);

}

int main(int argc, char **argv)
{
  //! The pointer to the GLFW window
  GLFWwindow *window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;
  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(512, 512, "CS475/CS675 Tutorial 5: Shading a Sphere", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  //! Make the window's context current
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    //Problem: glewInit failed, something is seriously wrong.
    std::cerr << "GLEW Init Failed : %s" << std::endl;
  }
  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  //Initialize GL state
  csX75::initGL();
  initBuffersGL();

  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
  {

    // Render here
    renderGL();

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

//-------------------------------------------------------------------------
