#ifndef _HNODE_HPP_
#define _HNODE_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "obj_model.hpp"

#include "gl_framework.hpp"



	// A simple class that represents a node in the hierarchy tree
	class HNode {
		//glm::vec4 * vertices;
		//glm::vec4 * colors;
		GLfloat tx,ty,tz,rx,ry,rz;

		//std::size_t vertex_buffer_size;
		//std::size_t color_buffer_size;

		//GLuint num_vertices;
		//GLuint vao,vbo;

		glm::mat4 rotation;
		glm::mat4 translation;
		
		std::vector<HNode*> children;
		HNode* parent;

        ObjModel* objModelPtr;

		void update_matrices();

	  public:
		HNode (HNode*, ObjModel*);
		//HNode (HNode* , glm::vec4*,  glm::vec4*,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat);

		void add_child(HNode*);
		void render(glm::mat4, glm::mat4, glm::mat3);
		void change_parameters(GLfloat,GLfloat,GLfloat,GLfloat,GLfloat,GLfloat);
		void render_tree(glm::mat4, glm::mat4, glm::mat3);
		void inc_rx();
		void inc_ry();
		void inc_rz();
		void dec_rx();
		void dec_ry();
		void dec_rz();
	};

	glm::mat4* multiply_stack(std::vector <glm::mat4> );

#endif
