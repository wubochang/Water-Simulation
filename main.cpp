#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ctime>
#include "camera.h"
#include "shader_s.h"
#include "Sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define N (128)
#define NN (N * N)
#define NUM_TRIANGLE ((N-1)*(N-1) * 2)
#define max(a,b) (a>b?a:b)

#define WATER_HEIGHT (0.2)

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const* path);
unsigned int loadTextureFromArray(float* arr, bool flag = false);
inline int ARRAY(int i, int j) { return j* N + i; }
inline void printVec(glm::vec4 v);
inline void printVec(glm::vec3 v);
unsigned int createEmptyTexture();
bool mouseHitWater(GLFWwindow* window, float* i, float* j);
void addDrop(float x, float y, float radius, float strength);
glm::vec3 getMouseRay(GLFWwindow* window);
bool mouseHitSphere(GLFWwindow* window);
void init_reflection();
void init_refraction();
void render_reflection();
void render_refraction();
void drawWall(Shader* ourShader, unsigned int VAO, glm::vec4 PlaneEquation);
void drawWallBlue();

//caemra

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseClick = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// lighting
glm::vec3 lightPos(1.0f, 3.0f, 2.0f);
bool lbutton_down = false;
int mouse_state = 0;
#define MOUSE_DRAGING (1)
#define MOUSE_ON_WATER (2)
#define MOUSE_ON_BALL (3)



// water
glm::mat4 waterModel = glm::mat4(1.0f);
float u[N*N];
float unew[N*N];
float v[N][N];
float sumN = 0;

// sphere
Sphere *curSphere;
glm::mat4 sphereModel = glm::mat4(1.0f);

// water reflection and refraction
GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
GLuint RflBuffer;
GLuint RfrBuffer;
GLuint RflDepthBuffer;
GLuint RfrDepthBuffer;
GLuint RflTexture;
GLuint RfrTexture;
//GLuint RflTextureID;
//GLuint RfrTextureID;
glm::vec4 PlaneEquation(1.0f);

float nextTime = 0;
float curTime = 0;
float clickInterval = 0.02;
bool toAddDrop = true;

Shader* wallShader = NULL;
Shader* wallBlueShaderPtr = NULL;
unsigned int VAO;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// begin init here
	// initial shaders
	Shader ourShader("basic_shader.vert", "basic_shader.frag");
	Shader waterShader("water.vert", "water.frag");
	Shader normalShader("normalmap.vert", "normalmap.frag");
	//Shader wallBlueShader("wall_blue.vert", "wall_blue.frag");
	Shader sphereShader("sphere.vert", "sphere.frag");
	Shader quadRflShader("ReflectionQuad.vertexshader", "Simple.fragmentshader");
	Shader quadRfrShader("RefractionQuad.vertexshader", "Simple.fragmentshader");
	Shader causticShader("caustic.vert", "caustic.frag");

	wallShader = &ourShader;
	//wallBlueShaderPtr = &wallBlueShader;

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, 0.0f, -1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f, 0.0f, -1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f, 0.0f, -1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	};
	unsigned int VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// initial reflection and refraction
	init_reflection();
	init_refraction();

	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	texture1 = loadTexture("tux-r.jpg");
	texture2 = loadTexture("brickwall.jpg");

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// init water vert
	float field_vertices[NN * 5];
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int dx = j * N + i;
			int tri_idx = dx * 5;
			field_vertices[tri_idx] = (float)i;
			field_vertices[tri_idx + 1] = 0.0f;
			field_vertices[tri_idx + 2] = (float)j;
			field_vertices[tri_idx + 3] = (float)i / (float)N;
			field_vertices[tri_idx + 4] = (float)j / (float)N;
		}
	}

	unsigned int indices[(N-1) * (N-1)* 6];
	for (int i = 0; i < N - 1; i++)
	{
		for (int j = 0; j < N - 1; j++)
		{
			unsigned int a = (j + 0) * N + (i + 0); // bottom left
			unsigned int b = (j + 0) * N + (i + 1); // bottom right
			unsigned int c = (j + 1) * N + (i + 1); // top right
			unsigned int d = (j + 1) * N + (i + 0); // top left
			int dx = j * (N-1) + i;
			indices[dx * 6 + 0] = a;
			indices[dx * 6 + 1] = b;
			indices[dx * 6 + 2] = c;

			indices[dx * 6 + 3] = a;
			indices[dx * 6 + 4] = c;
			indices[dx * 6 + 5] = d;
		}
	}

	// init u,v for water
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			int dx = j * N + i;
			u[dx] = sin((float)i / N * 4 * 3.1415) * cos((float)j / N * 4 * 3.1415);
			unew[dx] = u[dx];
			v[i][j] = 0.0f;
		}
	}


	unsigned int waterHeightMapTex = loadTextureFromArray(u);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterHeightMapTex);

	// render to texture, height and normal map
	unsigned int waterVBO, waterVAO, waterEBO;
	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	glGenBuffers(1, &waterEBO);

	glBindVertexArray(waterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(field_vertices) , field_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices , GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float) ));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// debug display
	GLfloat quad_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	unsigned int quadVBO, quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// set frame buffer, render to it
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a color attachment texture;
	unsigned int normalTexture0 = createEmptyTexture();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, normalTexture0, 0);

	// caustic textures
	unsigned int framebuffer2;
	glGenFramebuffers(1, &framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);

	unsigned int wallCausticTexture = createEmptyTexture();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, wallCausticTexture, 0);

	// shadow textures
	unsigned int framebuffer3;
	glGenFramebuffers(1, &framebuffer3);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3);

	unsigned int wallShadowTexture = createEmptyTexture();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, wallShadowTexture, 0);

	// gl setting
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	

	// active the shader
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("heightmapTex", 2);
	ourShader.setInt("textureCaustic", 4);

	waterShader.use();
	waterShader.setInt("heightmapTex", 2);
	waterShader.setInt("normalmapTex", 3);
	waterShader.setVec3("lightPos", lightPos);
	waterShader.setInt("RflTexture", 5);
	waterShader.setInt("RfrTexture", 6);

	normalShader.use();
	normalShader.setInt("heightmapTex", 2);

	/*wallBlueShader.use();
	wallBlueShader.setInt("heightmapTex", 2);
	wallBlueShader.setInt("textureCaustic", 4);*/

	causticShader.use();
	causticShader.setInt("heightmapTex", 2);
	causticShader.setInt("normalmapTex", 3);

	sphereShader.use();
	sphereShader.setVec3("lightPos", lightPos);

	quadRflShader.use();
	quadRflShader.setInt("myTextureSampler", 6);

	//quadRfrShader.use();
	//quadRfrShader.setInt("myTextureSampler", 5);



	curSphere = new Sphere();
	sphereModel = glm::mat4(1.0f);
	sphereModel = glm::scale(sphereModel, glm::vec3(0.2f, 0.2f, 0.2f));
	sphereModel = glm::translate(sphereModel, glm::vec3(1.0f, 100.5f, 0.0f));


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		waterModel = glm::mat4(1.0f);
		waterModel = glm::scale(waterModel, glm::vec3(1 / (float)N, 1.0f / (float)N, 1 / (float)N));
		waterModel = glm::translate(waterModel, glm::vec3(-0.5 * N, WATER_HEIGHT * N, -0.5 * N));

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glViewport(0, 0, N, N);
		sumN = 0;
		// update u and v and load into heightmap texture
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				float left = (i > 0) ? u[ARRAY(i - 1, j)] : u[ARRAY(0, j)];
				float right = (i < (N - 1)) ? u[ARRAY(i + 1, j)] : u[ARRAY(N - 1, j)];
				float up = (j > 0) ? u[ARRAY(i, j - 1)] : u[ARRAY(i, 0)];
				float down = (j < (N - 1)) ? u[ARRAY(i, j + 1)] : u[ARRAY(i, N - 1)];

				float f = 10 * ((left + right + down + up) - 4 * u[i + j*N]);
				{
					float max = 0.3;
					if (f > max) { f = max; }
					else if (f < -max) { f = -max; }
				}
				v[i][j] = v[i][j] + f * 0.18;
				v[i][j] *= 0.992f;
				int dx = j* N + i;
				unew[dx] = u[dx] + v[i][j] * 0.18;
				sumN += unew[dx];
			}
		}

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				int dx = j* N + i;
				u[dx] = unew[dx] - sumN / NN;
			}
		}
		waterHeightMapTex = loadTextureFromArray(u);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, waterHeightMapTex);

		normalShader.use();
		normalShader.setMat4("waterModel", waterModel);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffer(GL_COLOR_ATTACHMENT3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalTexture0);
		glBindVertexArray(waterVAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(*indices), GL_UNSIGNED_INT, 0);

		/*
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffer(GL_COLOR_ATTACHMENT3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalTexture0);

		normalShader.use();
		normalShader.setMat4("waterModel", waterModel);
		glBindVertexArray(waterVAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(*indices), GL_UNSIGNED_INT, 0);
		*/

		// calculate caustic texture
		causticShader.use();
		causticShader.setVec3("light", lightPos);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindVertexArray(waterVAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(*indices), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, wallCausticTexture);

		// reflection 
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		render_reflection();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, RflTexture);

		render_refraction();
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, RfrTexture);

		// input
		// -----
		processInput(window);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

#if 1
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.25f, 0.75f, 0.75f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glm::mat4 model = glm::mat4(1.0f);

		//ourShader.use();
		//ourShader.setMat4("view", view);
		//ourShader.setMat4("projection", projection);
		//ourShader.setVec3("camera_front", camera.Front);
		//ourShader.setMat4("model", model);

		//glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		drawWall(&ourShader, VAO, glm::vec4(0, -1, 0, 1));

		//drawWallBlue();
		/*
		wallBlueShader.use();
		wallBlueShader.setMat4("view", view);
		wallBlueShader.setMat4("projection", projection);
		wallBlueShader.setVec3("camera_front", camera.Front);
		wallBlueShader.setMat4("model", model);
		wallBlueShader.setMat4("invWaterModel", glm::inverse(waterModel));
		wallBlueShader.setInt("heightmapTex", 2);
		wallBlueShader.setVec3("light", lightPos);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/


		waterShader.use();
		waterShader.setMat4("view", view);
		waterShader.setMat4("projection", projection);
		waterShader.setMat4("model", waterModel);
		waterShader.setVec3("viewPos", camera.Position);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(waterVAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(*indices), GL_UNSIGNED_INT, 0);

		sphereShader.use();
		sphereShader.setMat4("view", view);
		sphereShader.setMat4("projection", projection);
		sphereShader.setMat4("model", sphereModel);
		sphereShader.setVec3("viewPos", camera.Position);
		glBindVertexArray(curSphere->sphereVAO);
		glDrawElements(GL_TRIANGLES, sizeof(curSphere->indices)/sizeof(*curSphere->indices), GL_UNSIGNED_INT, 0);


		quadRflShader.use();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

#endif

		glDeleteTextures(1, &waterHeightMapTex);
		glfwSwapBuffers(window);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		curTime = glfwGetTime();
		if (curTime > nextTime) {
			nextTime = curTime + clickInterval;
			toAddDrop = true;
		}

	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void drawWall(Shader* ourShader, unsigned int VAO, glm::vec4 PlaneEquation)
{
	wallShader->use();

	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	wallShader->setMat4("view", camera.GetViewMatrix());
	wallShader->setMat4("projection", projection);
	wallShader->setVec3("camera_front", camera.Front);
	wallShader->setVec4("ClipPlane", PlaneEquation);

	glm::mat4 model = glm::mat4(1.0f);
	wallShader->setMat4("model", model);
	wallShader->setMat4("invWaterModel", glm::inverse(waterModel));
	wallShader->setInt("heightmapTex", 2);
	wallShader->setVec3("light", lightPos);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawWallBlue()
{
	wallBlueShaderPtr->use();
	wallBlueShaderPtr->setMat4("view", view);
	wallBlueShaderPtr->setMat4("projection", projection);
	wallBlueShaderPtr->setVec3("camera_front", camera.Front);
	glm::mat4 model = glm::mat4(1.0f);
	wallBlueShaderPtr->setMat4("model", model);
	wallBlueShaderPtr->setMat4("invWaterModel", glm::inverse(waterModel));
	wallBlueShaderPtr->setInt("heightmapTex", 2);
	wallBlueShaderPtr->setVec3("light", lightPos);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!mouse_state)
	{
		lastX = xpos;
		lastY = ypos;
	}

	if (mouse_state == MOUSE_DRAGING)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	if (mouse_state == MOUSE_ON_WATER)
	{
		float hitI, hitJ;
		if (mouseHitWater(window, &hitI, &hitJ))
		{
			//std::cout << "creating drop at:(" << hitI << "," << hitJ << ")" << std::endl;
			addDrop(hitI, hitJ, 5, 0.05);
			//mouse_dragging_water = true;
		}
	}

	if (mouse_state == MOUSE_ON_BALL)
	{


	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

inline void printVec(glm::vec4 v)
{
	//std::cout << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << std::endl;
}

inline void printVec(glm::vec3 v)
{
	//std::cout << "(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
}


// glfw: whenever the mouse click, this callback is called
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	if (lbutton_down) {
		float hitI, hitJ;

		if (mouseHitSphere(window))
		{
			//std::cout << "hit sphere!" << std::endl;
			mouse_state = MOUSE_ON_BALL;
		}
		else if (mouseHitWater(window, &hitI, &hitJ))
		{
			//nextTime = time(NULL) + clickInterval;
			addDrop(hitI, hitJ, 5, 0.15);
			mouse_state = MOUSE_ON_WATER;
		}
		else
		{
			mouse_state = MOUSE_DRAGING;
		}
	}
	else
	{
		mouse_state = 0;
	}
}

unsigned int loadTexture(char const* path)
{
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		//std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	return texture1;
}

unsigned int loadTextureFromArray(float* arr, bool flag)
{
	unsigned int texture1;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, N, N, 0, GL_RED, GL_UNSIGNED_BYTE, arr); 
	if (!flag)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, N, N, 0, GL_RED, GL_FLOAT, arr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, N, N, 0, GL_RGB, GL_FLOAT, arr);
	glGenerateMipmap(GL_TEXTURE_2D);
	return texture1;
}

unsigned int createEmptyTexture()
{
	unsigned int t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, N, N, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return t;
}

glm::vec3 getMouseRay(GLFWwindow* window)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec4 mousePos = glm::vec4(xpos, ypos, 0.0f, 1.0f);

	float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	glm::vec3 ray_wor = (glm::inverse(camera.GetViewMatrix()) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor;
}

bool mouseHitWater(GLFWwindow* window, float * i, float * j)
{
	glm::vec3 ray_wor = getMouseRay(window);

	glm::vec3 cen(waterModel * glm::vec4(0, 0, 0, 1));
	glm::vec3 norm(0, 1, 0);
	float dist = glm::dot((cen - camera.Position), norm) / glm::dot(norm, ray_wor);
	auto hitPoint = camera.Position + dist * ray_wor;
	auto hp_waterSpace = glm::inverse(waterModel) * glm::vec4(hitPoint, 1);

	*i = (hp_waterSpace.x);
	*j = (hp_waterSpace.z);
	if (*i > 127 || *i < 0 || *j > 127 || *j < 0)
		return false;

	//std::cout << "hit at y = " << hp_waterSpace.y << std::endl;
	return true;
}

bool mouseHitSphere(GLFWwindow* window)
{
	glm::vec3 ray_wor = getMouseRay(window);

	printVec(ray_wor);
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(sphereModel, scale, rotation, translation, skew, perspective);

	auto l = translation - camera.Position;
	float tc = abs(glm::dot(l, ray_wor));
	float dist = glm::length(l - tc * ray_wor);
	if(dist > scale.x) // no hit
		return false;
	return true;
}

void addDrop(float x, float y, float radius, float strength)
{
	if (toAddDrop) {
		toAddDrop = false;

		float innerCnt = 0;
		float outerWeight = 0;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				glm::vec2 coord(i, j);
				glm::vec2 center(x, y);
				float dist = glm::length(coord - center);
				float drop = 0;
				if (dist > radius && dist < 2 * radius)
				{
					outerWeight += 1 / dist;
				}
				else if (dist < radius)
				{
					drop = -sqrt(1 - (dist / radius) * (dist / radius));
					drop *= strength;
					innerCnt += drop;
					v[i][j] += drop;
				}
			}
		}

		float actualOut = 0;

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				glm::vec2 coord(i, j);
				glm::vec2 center(x, y);
				float dist = glm::length(coord - center);
				float drop = 0;
				if (dist > radius && dist < 2 * radius)
				{
					drop = -innerCnt * (1 / dist) / outerWeight;
					actualOut += drop;
					v[i][j] += drop * 2;
				}
			}
		}

		//std::cout << "inner:" << innerCnt << " outer:" << actualOut << std::endl;
		//std::cout << "sum N is " << sumN << std::endl;
	}
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	glGenRenderbuffers(1, &RflBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, RflBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	glGenRenderbuffers(1, &RfrDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, RfrDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
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

	camera.Pitch *= -1;
	camera.updateCameraVectors();
	camera.Position.y = 2 * (WATER_HEIGHT) - camera.Position.y;
	camera.Up.y *= -1;

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, RflBuffer);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// Render on the whole framebuffer, complete from the lower left corner to the upper right

	glClearColor(0.25f, 0.75f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render objects
	drawWall(wallShader, VAO, glm::vec4(0, 1, 0, -WATER_HEIGHT));

	camera.Pitch *= -1;
	camera.Position.y = 2 * (WATER_HEIGHT) - camera.Position.y;
	camera.updateCameraVectors();

	glDisable(GL_CLIP_DISTANCE0);
}

void render_refraction()
{
	//enable clipping plane
	glEnable(GL_CLIP_DISTANCE0);

	camera.Position.y *= 0.9;

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, RfrBuffer);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	// Render on the whole framebuffer, complete from the lower left corner to the upper right

	glClearColor(0.25f, 0.75f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render objects
	drawWall(wallShader, VAO, glm::vec4(0, -1, 0, WATER_HEIGHT));
	//drawWallBlue();

	camera.Position.y /= 0.9;

	glDisable(GL_CLIP_DISTANCE0);
}