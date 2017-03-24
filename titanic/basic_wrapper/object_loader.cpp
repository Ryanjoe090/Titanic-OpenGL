/*
 Lab3start
 Creates a cube and a sphere
 Includes rotation, scaling, translation, view and perspective transformations,
 and is setup for you to add lighting because the objects have normals and colours defined
 for all vertices.
 Feel free to either use this example as a start to lab3 or extract bits and add to
 an example of your own to practice implementing per-vertex lighting.
 Iain Martin October 2014
*/


/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glloadD.lib")
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include "SOIL.h"
#include "particle_object.h"
#include <iostream>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "object_ldr.h"
#include "terrain_object.h"

GLuint program, particle_program;		/* Identifier for the shader prgoram */
GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, scale, z, y;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLfloat vx, vy, vz;
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;	//Define the resolution of the sphere object

/* Uniforms*/
GLuint modelID, viewID, projectionID, textureID, textureID2;
GLuint terrianOnID, terrainmode;
GLuint textureOnId, texturemode;

GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/

object_ldr titanic;
particle_object smoke, smoke2, smoke3;

terrain_object *heightfield;
int octaves;
GLfloat perlin_scale, perlin_frequency;
GLfloat land_size;




/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw)
{
	/* Set the object transformation controls to their initial values */
	x = 0.05f;
	y = 0;
	z = 0;
	angle_x = angle_y = angle_z = 0;
	vx = vy = vz = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0;
	scale = 1.f;
	aspect_ratio = 1.3333f;
	terrainmode = 0;
	texturemode = 0;

	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	printf("CONTROLS:\nRotate: E, R\nSink: T, Y\nScale/Zoom: A, S\nSwitch to World War 1 Dazzle Camouflage: M\nCamera \'7890OP\'\n");

	/* Load and create our titanic object*/
	titanic.load_obj("titanic5.obj");

	/* Calculate vertex normals using cross products from the surrounding faces*/
	/* A better way to do this would be to generate the vertex normals in Blender and
	/* then extract the vertex normals from the face definitions and use that to create an
	/* accurate array of veretx normals */
	//monkey.smoothNormals();
	titanic.createObject();

	//TERRAIN
	octaves = 6;
	perlin_scale = 2.6f;
	perlin_frequency = 6.f;
	land_size = 150.f;
	heightfield = new terrain_object(octaves, perlin_frequency, perlin_scale);
	heightfield->createTerrain(100, 100, land_size, land_size);
	heightfield->createObject();

	
	/* Load and build the vertex and fragment shaders */
	try
	{
		program = glw->LoadShader("object_loader.vert", "object_loader.frag");
		particle_program = glw->LoadShader("particle_object.vert", "particle_object.frag");
	}
	catch (std::exception &e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	try{
		textureID = SOIL_load_OGL_texture("titanicPDN.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y |  SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		//textureID = SOIL_load_OGL_texture("titanicheeky.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);


		if (textureID == 0)
		{
			printf("SOIL loading error: '%s'\n", SOIL_last_result());
		}

		int loc = glGetUniformLocation(program, "tex1");
		if (loc >= 0) glUniform1i(loc, 0);
	}
	catch (std::exception &e)
	{
		printf("\nImage file Loading Failed");
	}

	try{
		textureID2 = SOIL_load_OGL_texture("titanicPDNDazzle.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		//textureID = SOIL_load_OGL_texture("titanicheeky.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);


		if (textureID2 == 0)
		{
			printf("SOIL loading error: '%s'\n", SOIL_last_result());
		}

		int loc = glGetUniformLocation(program, "tex2");
		if (loc >= 0) glUniform1i(loc, 0);
	}
	catch (std::exception &e)
	{
		printf("\nImage file Loading Failed");
	}

	/* Define uniforms to send to vertex shader */
	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projectionID = glGetUniformLocation(program, "projection");
	terrianOnID = glGetUniformLocation(program, "terrain");
	textureOnId = glGetUniformLocation(program, "texturemode");
	smoke.create(particle_program);
	smoke2.create(particle_program);
	smoke3.create(particle_program);
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	/* Make the compiled shader program current */
	glUseProgram(program);

	// Define the model transformations for the cube
	terrainmode = 0;
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(x, y, z - 4.f));
	model = glm::scale(model, glm::vec3(scale, scale, scale));//scale equally in all axis
	model = glm::rotate(model, -angle_x, glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	model = glm::rotate(model, -angle_y, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model = glm::rotate(model, -angle_z, glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
	model = glm::translate(model, glm::vec3(x, y, z));
	//model = glm::rotate(model, 90.f, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(30.0f, aspect_ratio, 0.1f, 100.0f);



	glm::mat4 View = glm::lookAt(
			
		glm::vec3(0,0,30),
			glm::vec3(0,0,0),			
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);

	View = glm::rotate(View, -vx + 10.f, glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	View = glm::rotate(View, -vy + 180.f, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	View = glm::rotate(View, -vz, glm::vec3(0, 0, 1));
	//glm::mat4 View = glm::translate(model, glm::vec3(x, y, z - 35.f));
	
	// Send our uniforms variables to the currently bound shader,
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);
	glUniform1ui(terrianOnID, terrainmode);
	glUniform1ui(textureOnId, texturemode);

	//Draw titanic
	if (texturemode == 0)
	{
		titanic.drawObject(&textureID);
	}
	else
	{
		titanic.drawObject(&textureID2);
	}


	/* Define the model transformations for our sphere */
	terrainmode = 1;
	glm::mat4 modelT = glm::mat4(1.0f);
	modelT = glm::translate(modelT, glm::vec3(0, 0.f, -3.f));
	modelT = glm::scale(modelT, glm::vec3(scale * 2, scale * 2, scale * 2));//scale equally in all axis
	modelT = glm::rotate(modelT, 0.f, glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	modelT = glm::rotate(modelT,90.f, glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	modelT = glm::rotate(modelT,0.f, glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelT[0][0]);
	glUniform1ui(terrianOnID, terrainmode);
	heightfield->drawObject(2);
	/* Draw our sphere */
	//drawSphere();
	//heightfield->drawObject(drawmode);
	terrainmode = 0;
	/* Modify our animation variables */


	angle_x += angle_inc_x;
	angle_y += angle_inc_y;
	angle_z += angle_inc_z;


	/* Update and draw our particle animation */
	smoke.drawParticles(Projection, View * model, 0.7f);
	smoke2.drawParticles(Projection, View * model, 2.6f);
	smoke2.drawParticles(Projection, View * model, -0.7f);
	glDisableVertexAttribArray(2);

}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f*4.f) / ((float)h / 480.f*3.f);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/
	//if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//if (key == 'Q') angle_inc_x -= 0.05f;
	//if (key == 'W') angle_inc_x += 0.05f;
	if (key == 'E') angle_inc_y -= 0.05f;
	if (key == 'R') angle_inc_y += 0.05f;
	if (key == 'T') angle_inc_z -= 0.05f;
	if (key == 'Y') angle_inc_z += 0.05f;
	if (key == 'A') scale -= 0.02f;
	if (key == 'S') scale += 0.02f;
	if (key == 'Z') x -= 0.5f;
	if (key == 'X') x += 0.5f;
	if (key == 'C') y -= 0.05f;
	if (key == 'V') y += 0.05f;
	if (key == 'B') z -= 0.05f;
	if (key == 'N') z += 0.05f;
	if (key == '7') vx -= 1.f;
	if (key == '8') vx += 1.f;
	if (key == '9') vy -= 1.f;
	if (key == '0') vy += 1.f;
	if (key == 'O') vz -= 1.f;
	if (key == 'P') vz += 1.f;

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == 'N' && action != GLFW_PRESS)
	{
		drawmode ++;
		if (drawmode > 2) drawmode = 0;
	}

	if (key == 'M' && action != GLFW_PRESS)
	{
		texturemode++;
		if (texturemode > 1) texturemode = 0;
	}
}

/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper *glw = new GLWrapper(1024, 768, "Lab5: Fun with texture");;

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}

