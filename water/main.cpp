// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

// Include GLEW. 
//Always include it before gl.h and glfw.h, since it's a bit magic.
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"

using namespace glm;
using namespace std;

GLFWwindow* window; 
int width=1024;
int height=768;

//IDs
GLuint VertexArrayID;
GLuint programID[3];
GLuint TextureID[3];//a handle for sampler
GLuint Texture[2];//load the texture
GLuint MatrixID[3];//MVP
GLuint ViewMatrixID[2];//V
GLuint ModelMatrixID[2];//M
GLuint LightID[2];

//Buffers
GLuint vertexbuffer[3];
GLuint uvbuffer[3];
GLuint normalbuffer[3];

//obj fnames
std::string objfiles[2] = {"wall.obj","water.obj"};

//uniform informations
mat4 ProjectionMatrix;
mat4 ViewMatrix;
mat4 ModelMatrix;
mat4 MVP;
vec3 lightPos;

// The fullscreen quad's FBO
static const GLfloat quad_data[] = { 
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f,
};
GLuint ClipID;
GLuint quad_VertexArrayID;
GLuint quad_vertexbuffer;
GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
GLuint quad_programID[2];
GLuint quad_textureID[2];
//water reflect and refract
GLuint RflBuffer;
GLuint RfrBuffer;
GLuint RflDepthBuffer;
GLuint RfrDepthBuffer;
GLuint RflTexture;
GLuint RfrTexture;
GLuint RflTextureID;
GLuint RfrTextureID;
//dudv map
GLuint DuDvTexture;
GLuint DuDvTextureID;
float MoveFactor=0;
GLuint MoveFactorID;
//render functions
void render_wall(glm::vec4 PlaneEquation);
void render_water();
void render_skybox();
void render_refraction();
void render_reflection();
void render2screen();
void render_scene();
//init functions
void init_reflection();
void init_refraction();

void init()
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	//VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//texture
	// Load the texture using any two methods
	//GLuint Texture = loadBMP_custom("uvtemplate.bmp");
	Texture[0] = loadDDS("tiles.dds");
    Texture[1] = loadDDS("sky.dds");
    DuDvTexture = loadDDS("dudv.dds");

    //Wall
	programID[0] = LoadShaders("Shading.vertexshader", "Shading.fragmentshader");
	//Water
	programID[1] = LoadShaders("Water.vertexshader", "Water.fragmentshader");
	//Sky
	programID[2] = LoadShaders("Simple.vertexshader", "Simple.fragmentshader");
	// Get a handle for our "MVP" uniform,"myTextureSampler" uniform
	//wall
	TextureID[0]  = glGetUniformLocation(programID[0], "myTextureSampler");
	MatrixID[0] = glGetUniformLocation(programID[0], "MVP");
	ViewMatrixID[0] = glGetUniformLocation(programID[0], "V");
	ModelMatrixID[0] = glGetUniformLocation(programID[0], "M");
	LightID[0] = glGetUniformLocation(programID[0], "LightPosition_worldspace");
	ClipID = glGetUniformLocation(programID[0], "ClipPlane");
	//water
	RflTextureID  = glGetUniformLocation(programID[1], "ReflectionSampler");
	RfrTextureID  = glGetUniformLocation(programID[1], "RefractionSampler");
	MatrixID[1] = glGetUniformLocation(programID[1], "MVP");
	ViewMatrixID[1] = glGetUniformLocation(programID[1], "V");
	ModelMatrixID[1] = glGetUniformLocation(programID[1], "M");
	LightID[1] = glGetUniformLocation(programID[1], "LightPosition_worldspace");
	DuDvTextureID = glGetUniformLocation(programID[1], "DuDvSampler");
	MoveFactorID = glGetUniformLocation(programID[1], "MoveFactor");
	//sky
	TextureID[2]  = glGetUniformLocation(programID[2], "myTextureSampler");
	MatrixID[2] = glGetUniformLocation(programID[2], "MVP");
	
	//geneate the buffers
	glGenBuffers(3, vertexbuffer);
	glGenBuffers(3, uvbuffer);
	glGenBuffers(3, normalbuffer);
	//load the 1st obj file
	std::vector< glm::vec3 > vertices_wall;
	std::vector< glm::vec2 > uvs_wall;
	std::vector< glm::vec3 > normals_wall; 

	bool res1 = loadOBJ("wall.obj", vertices_wall, uvs_wall, normals_wall);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices_wall.size() * sizeof(glm::vec3), &vertices_wall[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, uvs_wall.size() * sizeof(glm::vec2), &uvs_wall[0], GL_STATIC_DRAW);
	
 	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[0]);
 	glBufferData(GL_ARRAY_BUFFER, normals_wall.size() * sizeof(glm::vec3), &normals_wall[0], GL_STATIC_DRAW);

 	//load the 2nd obj file
 	std::vector< glm::vec3 > vertices_water;
	std::vector< glm::vec2 > uvs_water;
	std::vector< glm::vec3 > normals_water; 

	bool res2 = loadOBJ("water.obj", vertices_water, uvs_water, normals_water);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices_water.size() * sizeof(glm::vec3), &vertices_water[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs_water.size() * sizeof(glm::vec2), &uvs_water[0], GL_STATIC_DRAW);
	
 	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[1]);
 	glBufferData(GL_ARRAY_BUFFER, normals_water.size() * sizeof(glm::vec3), &normals_water[0], GL_STATIC_DRAW);

 	//load the 3rd obj file
 	std::vector< glm::vec3 > vertices_sky;
	std::vector< glm::vec2 > uvs_sky;
	std::vector< glm::vec3 > normals_sky; 

	bool res3 = loadOBJ("sky.obj", vertices_sky, uvs_sky, normals_sky);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, vertices_sky.size() * sizeof(glm::vec3), &vertices_sky[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, uvs_sky.size() * sizeof(glm::vec2), &uvs_sky[0], GL_STATIC_DRAW);
	
 	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[2]);
 	glBufferData(GL_ARRAY_BUFFER, normals_sky.size() * sizeof(glm::vec3), &normals_sky[0], GL_STATIC_DRAW);

 	//initialize the frame buffers
 	init_refraction();
 	init_reflection();

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Frame buffer Error!\n");
		return;
	}

	// The fullscreen quad's FBO
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

	// reflection quad
	quad_programID[0] = LoadShaders( "ReflectionQuad.vertexshader", "Simple.fragmentshader" );
	quad_textureID[0] = glGetUniformLocation(quad_programID[0], "myTextureSampler");

	//refraction quad
	quad_programID[1] = LoadShaders( "RefractionQuad.vertexshader", "Simple.fragmentshader" );
	quad_textureID[1] = glGetUniformLocation(quad_programID[1], "myTextureSampler");
}

void render()
{
	// Compute the MVP matrix from keyboard and mouse input
	computeMatricesFromInputs(true);

	ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix (eye,lookat,up)
	// move the camera below the water
	ViewMatrix = lookAt(vec3(4,-1.8,4),
						vec3(0,0.6,0), 
						vec3(0,-1,0));
	ModelMatrix = getModelMatrix();
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	lightPos = glm::vec3(4,4,4);
	render_reflection();

	computeMatricesFromInputs(false);
	//recover the original viewmatrix
	ViewMatrix = lookAt(vec3(4,3,4),
						vec3(0,0.6,0), 
						vec3(0,1,0));
	ModelMatrix = getModelMatrix();
	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	render_refraction();
	render2screen();

	render_scene();
}

void clean()
{
	glDeleteBuffers(3, vertexbuffer);
	glDeleteBuffers(3, uvbuffer);
	glDeleteBuffers(3, normalbuffer);
	glDeleteProgram(programID[0]);
	glDeleteProgram(programID[1]);
	glDeleteProgram(programID[2]);
	glDeleteTextures(2, Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	glDeleteFramebuffers(1, &RflBuffer);
	glDeleteTextures(1, &RflTexture);
	glDeleteFramebuffers(1, &RfrBuffer);
	glDeleteTextures(1, &RfrTexture);
	glDeleteRenderbuffers(1, &RflDepthBuffer);
	glDeleteRenderbuffers(1, &RfrDepthBuffer);
	glDeleteBuffers(1, &quad_vertexbuffer);
	glDeleteVertexArrays(1, &quad_VertexArrayID);
}

int main(){
	// Initialise GLFW
	if( !glfwInit() )
	{
	    fprintf( stderr, "Failed to initialize GLFW\n" );
	    return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	
	window = glfwCreateWindow( width, height, "water", NULL, NULL);
	if( window == NULL ){
	    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window);

	 // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
	    fprintf(stderr, "Failed to initialize GLEW\n");
	    return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	init();

	do{
		render();
	    // Swap buffers
	    glfwSwapBuffers(window);
	    glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0 );

	clean();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

}

void render_wall(glm::vec4 PlaneEquation)
{
	//the walls

	// Use our shader
	glUseProgram(programID[0]);

	//printf(to_string(mvp).c_str());
	// Send our transformation to the currently bound shader, 
	//in the "MVP" uniform
	glUniformMatrix4fv(MatrixID[0], 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID[0], 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID[0], 1, GL_FALSE, &ViewMatrix[0][0]);

	glUniform3f(LightID[0], lightPos.x, lightPos.y, lightPos.z);

	glUniform4f(ClipID,PlaneEquation.x,PlaneEquation.y,PlaneEquation.z,PlaneEquation.w);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID[0], 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glVertexAttribPointer(
	   0,                  // attribute 0. 
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	// 2nd attribute buffer : uv coord
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[0]);
	glVertexAttribPointer(
	    1,                                // attribute 1.
	    2,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	// Starting from vertex 0; 3 vertices total -> 1 triangle
	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[0]);
	glVertexAttribPointer(
	    2,                                // attribute
	    3,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 6*3); 

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

}

void render_water()
{

	//the waterpuafd;ifj;odsifodisfjdoisfmkisdsoifjosdifs
	// Use our shader
	glUseProgram(programID[1]);
	// Send our transformation to the currently bound shader, 
	//in the "MVP" uniform
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	

	glUniformMatrix4fv(MatrixID[1], 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID[1], 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID[1], 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniform3f(LightID[1], lightPos.x, lightPos.y, lightPos.z);
	MoveFactor += 0.00001;
	if(MoveFactor>=1)
		MoveFactor = 0;
	glUniform1f(MoveFactorID, MoveFactor);
	// Bind our texture in Texture Unit 2,3,4
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RflTexture);
	glUniform1i(RflTextureID, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, RfrTexture);
	glUniform1i(RfrTextureID, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, DuDvTexture);
	glUniform1i(DuDvTextureID, 4);


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	glVertexAttribPointer(
	   0,                  // attribute 0. 
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	// 2nd attribute buffer : uv coord
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[1]);
	glVertexAttribPointer(
	    1,                                // attribute 1.
	    2,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	// Starting from vertex 0; 3 vertices total -> 1 triangle
	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[1]);
	glVertexAttribPointer(
	    2,                                // attribute
	    3,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 2*3);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

void render_skybox()
{
	//the skybox
	// Use our shader
	glUseProgram(programID[2]);
	// Send our transformation to the currently bound shader, 
	//in the "MVP" uniform
	mat4 ModelMatrix_sky = glm::scale(glm::mat4(1.0),glm::vec3(100.0f,100.0f,100.0f));
    //ModelMatrix_sky = glm::rotate(ModelMatrix_sky,90.0f,glm::vec3(1.0,0.0,0.0));
	mat4 MVP_sky = ProjectionMatrix * ViewMatrix * ModelMatrix_sky;
	glUniformMatrix4fv(MatrixID[2], 1, GL_FALSE, &MVP_sky[0][0]);

	// Bind our texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	// Set our "myTextureSampler" sampler to use Texture Unit 1
	glUniform1i(TextureID[2], 1);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
	glVertexAttribPointer(
	   0,                  // attribute 0. 
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	// 2nd attribute buffer : uv coord
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[2]);
	glVertexAttribPointer(
	    1,                                // attribute 1.
	    2,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	// Starting from vertex 0; 3 vertices total -> 1 triangle
	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[2]);
	glVertexAttribPointer(
	    2,                                // attribute
	    3,                                // size
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized?
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 11*3);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

void render_scene()
{
	//render scene objects
	render_wall(glm::vec4(0,-1.0,0,2.0));
	render_skybox();
	render_water();
}


void init_reflection()
{
	// The reflection frame buffer
	glGenFramebuffers(1, &RflBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RflBuffer);

	// The texture we're going to render to
	glGenTextures(1, &RflTexture);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, RflTexture);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	glGenRenderbuffers(1, &RflBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, RflBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RflBuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RflTexture, 0);
	// Set the list of draw buffers.
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}

void init_refraction()
{
	// The reflection frame buffer
	glGenFramebuffers(1, &RfrBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RfrBuffer);

	// The texture we're going to render to
	glGenTextures(1, &RfrTexture);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, RfrTexture);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	glGenRenderbuffers(1, &RfrDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, RfrDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RfrDepthBuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RfrTexture, 0);
	// Set the list of draw buffers.
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
}

void render_reflection()
{
	//enable clipping plane
	glEnable(GL_CLIP_DISTANCE0);

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, RflBuffer);
	glViewport(0,0,width,height); 
	// Render on the whole framebuffer, complete from the lower left corner to the upper right

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render objects
	render_wall(glm::vec4(0,1.0,0,-0.6));
	render_skybox();
	//render_water();

	glDisable(GL_CLIP_DISTANCE0);
}

void render_refraction()
{
	//enable clipping plane
	glEnable(GL_CLIP_DISTANCE0);

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, RfrBuffer);
	glViewport(0,0,width,height); 
	// Render on the whole framebuffer, complete from the lower left corner to the upper right

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render objects
	render_wall(glm::vec4(0,-1.0,0,0.6));
	render_skybox();
	//render_water();

	glDisable(GL_CLIP_DISTANCE0);
}

void render2screen()
{
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Render on the whole framebuffer, complete from the lower 
    //left corner to the upper right
	glViewport(0,0,width,height);

	// Clear the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(quad_programID[0]);

	// Bind our texture in Texture Unit 2
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, RflTexture);
	// Set our "renderedTexture" sampler to use Texture Unit 2
	glUniform1i(quad_textureID[0], 2);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);

	// Use our shader
	glUseProgram(quad_programID[1]);

	// Bind our texture in Texture Unit 3
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, RfrTexture);
	// Set our "renderedTexture" sampler to use Texture Unit 2
	glUniform1i(quad_textureID[1], 3);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}