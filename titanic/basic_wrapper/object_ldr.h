#pragma once
/* object_ldr.h
   Example class to show the start of an .obj mesh obkect file
   loader
   Iain Martin November 2014
*/

#pragma once

#include "wrapper_glfw.h"
#include "SOIL.h"
#include <vector>
#include <glm/glm.hpp>

class object_ldr
{
public:
	object_ldr();
	~object_ldr();

	void load_obj(const char* filename);
	void drawObject(GLuint* texturePointer);
	void createObject();
	void smoothNormals();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> verticesFinal;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> normalsFinal;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec2> textureCoordsFinal;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<GLushort> elements;
	std::vector<GLushort> textureElements;

	GLuint vbo_mesh_vertices;
	GLuint vbo_mesh_normals;
	GLuint vbo_tangents;
	GLuint vbo_bitangents;
	GLuint ibo_mesh_elements;
	GLuint ibo_texture_elements;
	GLuint vbo_mesh_textureCoords;
	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_text_coord;
};

