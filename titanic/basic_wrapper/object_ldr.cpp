/* object_ldr.cpp
Example class to show the start of an .obj mesh obkect file
loader
Iain Martin November 2014
Extended to handle full wavefront obj. file

This is a wrapper class based around the code taken from :
http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ
I made a few changes to fit in with my vertex shaders and added the code to
bind the vertex buffers.
Uses std::vector class as the containor for the array of glm::vec3 types
*/

#include "object_ldr.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>


/* I don't like using namespaces in header files but have less issues with them in
   seperate cpp files */
using namespace std;

/* Define the vertex attributes for vertex positions and normals. 
   Make these match your application and vertex shader
   You might also want to add colours and texture coordinates */
object_ldr::object_ldr()
{
	attribute_v_coord = 0;
	attribute_v_normal = 1;
}


object_ldr::~object_ldr()
{
}

/* Load the object, parsing the file. 
For every line begining with 'v', it adds an extra glm::vec3 to the std::vector containor 
For every line beginning with 'f', it adds the "face" indices to the array of indices
Then it calculates flat shaded normals, i.e calculates the face normal and applies that to 
all vertices in the face.

This function could be improved by extending the parsing to cope with face definitions with
normals defined.
*/
void object_ldr::load_obj(const char* filename) {
	ifstream in(filename, ios::in);
	if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }

	string line;
	while (getline(in, line)) 
	{
		if (line.substr(0, 2) == "v ") 
		{
			istringstream s(line.substr(2));
			glm::vec3 v; s >> v.x; s >> v.y; s >> v.z; 
			vertices.push_back(v);
		}
		else if (line.substr(0, 3) == "vt ")
		{
			istringstream s(line.substr(3));
			glm::vec2 vt; s >> vt.x; s >> vt.y;
			textureCoords.push_back(vt);
		}
		else if (line.substr(0, 3) == "vn ")
		{
			istringstream s(line.substr(3));
			glm::vec3 vn; s >> vn.x; s >> vn.y; s >> vn.z;
			normals.push_back(vn);
		}
		else if (line.substr(0, 2) == "f ") 
		{
			string sS = line.substr(2);
			std::replace(sS.begin(), sS.end(), '/', ' ');
			istringstream s(sS);
			//istringstream s(line.substr(2));
			GLushort a, b, c, d, e, f, g, h, i;
			s >> a; //first vertex element
			//s.ignore(256, ' '); //skip till next set
			s >> b; 
			//s.ignore(256, ' '); //skip till next set
			s >> c;
			//s.ignore(256, ' '); //skip till next set
			s >> d; s >> e; s >> f; s >> g; s >> h; s >> i;
			a--; d--; g--;
			b--; e--; h--;
			c--; f--; i--;
			/*elements.push_back(a); elements.push_back(d); elements.push_back(g);
			textureElements.push_back(b), textureElements.push_back(e), textureElements.push_back(h);*/
			verticesFinal.push_back(vertices.at(a)); verticesFinal.push_back(vertices.at(d)); verticesFinal.push_back(vertices.at(g));
			textureCoordsFinal.push_back(textureCoords.at(b)); textureCoordsFinal.push_back(textureCoords.at(e)); textureCoordsFinal.push_back(textureCoords.at(h));
			normalsFinal.push_back(normals.at(c)); normalsFinal.push_back(normals.at(f)); normalsFinal.push_back(normals.at(i));
			//reverse(normalsFinal.begin(), normalsFinal.end());
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}

	/*normalsFinal.resize(verticesFinal.size(), glm::vec3(0.0, 0.0, 0.0));
	for (GLuint i = 0; i < elements.size(); i += 3) {
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
			glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;
	}*/
	//FOLLOWING ADAPTED OFF http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
	for (int i = 0; i < verticesFinal.size(); i += 3)
	{
		glm::vec3 & v0 = verticesFinal[i + 0];
		glm::vec3 & v1 = verticesFinal[i + 1];
		glm::vec3 & v2 = verticesFinal[i + 2];

		// Shortcuts for UVs
		glm::vec2 & uv0 = textureCoordsFinal[i + 0];
		glm::vec2 & uv1 = textureCoordsFinal[i + 1];
		glm::vec2 & uv2 = textureCoordsFinal[i + 2];

		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
}


/* Copy the vertices, normals and element indices into vertex buffers */
void object_ldr::createObject()
{
	/* Generate the vertex buffer object */
	/*glGenBuffers(1, &vbo_mesh_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	glGenBuffers(1, &vbo_mesh_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
	glBufferData(GL_ARRAY_BUFFER, verticesFinal.size() * sizeof(glm::vec3), &(verticesFinal[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* Store the normals in a buffer object */
	glGenBuffers(1, &vbo_mesh_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
	glBufferData(GL_ARRAY_BUFFER, normalsFinal.size() * sizeof(glm::vec3), &(normalsFinal[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// Generate a buffer for the indices
	//glGenBuffers(1, &ibo_mesh_elements);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()* sizeof(GLushort), &(elements[0]), GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate a buffer for the texture
	glGenBuffers(1, &vbo_mesh_textureCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_textureCoords);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), &(textureCoords[0]), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, textureCoordsFinal.size() * sizeof(glm::vec2), &(textureCoordsFinal[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_tangents);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &(tangents[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_bitangents);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bitangents);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &(bitangents[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	////Generate buffers for the texture indices. THIS TURNED OUT TO BE BARKING UP THE VERY WRONG TREE
	//glGenBuffers(1, &ibo_texture_elements);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_texture_elements);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, textureElements.size()* sizeof(GLushort), &(textureElements[0]), GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

/* Enable vertex attributes and draw object
Could improve efficiency by moving the vertex attribute pointer functions to the
create object but this method is more general 
This code is almost untouched fomr the tutorial code except that I changed the
number of elements per vertex from 4 to 3*/
void object_ldr::drawObject(GLuint* texturePointer)
{
	int size;	// Used to get the byte size of the element (vertex index) array

	// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
	glEnableVertexAttribArray(attribute_v_coord);
	glVertexAttribPointer(
		attribute_v_coord,  // attribute index
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
	glEnableVertexAttribArray(attribute_v_normal);
	glVertexAttribPointer(
		attribute_v_normal, // attribute
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_textureCoords);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
	glVertexAttribPointer(
		3,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_texture_elements);
	//glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizeText);


	glBindTexture(GL_TEXTURE_2D, *texturePointer);
	glDrawArrays(GL_TRIANGLES, 0, verticesFinal.size() * 3);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements); 
	//glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);	
	//glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
}

/* This is the smooth normals function given in the tutorial code */
void object_ldr::smoothNormals()
{
	std::vector<GLuint> nb_seen;
	normalsFinal.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	nb_seen.resize(vertices.size(), 0);
	for (int i = 0; i < elements.size(); i += 3) {
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(verticesFinal[ib]) - glm::vec3(verticesFinal[ia]),
			glm::vec3(verticesFinal[ic]) - glm::vec3(verticesFinal[ia])));

		int v[3];  v[0] = ia;  v[1] = ib;  v[2] = ic;
		for (int j = 0; j < 3; j++) {
			GLushort cur_v = v[j];
			nb_seen[cur_v]++;
			if (nb_seen[cur_v] == 1) {
				normalsFinal[cur_v] = normal;
			}
			else {
				// average
				normalsFinal[cur_v].x = normalsFinal[cur_v].x * (1.0 - 1.0 / nb_seen[cur_v]) + normal.x * 1.0 / nb_seen[cur_v];
				normalsFinal[cur_v].y = normalsFinal[cur_v].y * (1.0 - 1.0 / nb_seen[cur_v]) + normal.y * 1.0 / nb_seen[cur_v];
				normalsFinal[cur_v].z = normalsFinal[cur_v].z * (1.0 - 1.0 / nb_seen[cur_v]) + normal.z * 1.0 / nb_seen[cur_v];
				normalsFinal[cur_v] = glm::normalize(normalsFinal[cur_v]);
			}
		}
	}

}


