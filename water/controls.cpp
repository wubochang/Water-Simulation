#include <stdio.h>
// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 MyModelMatrix;

glm::mat4 getModelMatrix(){
	return MyModelMatrix;
}

double position[2] = {0,0};
int leftbutton = 0;    /* 1 if pressed, 0 if not */
int rightbutton = 0;

/* state of the world */
float r_arr[3] = {0.0,0.0,0.0};
float s_arr[3] = {1.0,1.0,1.0};
float speed = 200.0f; // 200 units / second

void scroll(GLFWwindow* window,double x,double y){
	s_arr[0]+=0.0005 * y * speed ;
	s_arr[1]+=0.0005 * y * speed ;
	s_arr[2]+=0.0005 * y * speed ;
}

void computeMatricesFromInputs(bool invert){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	//printf("%f,%f\n",position[0],position[1]);

	// Get mouse position
	glfwGetCursorPos(window, &position[0], &position[1]);

	if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS)
	{
		double xpos,ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		double mousedelta[2] = {xpos-position[0], ypos-position[1]};
		//r_arr[0]+= speed * deltaTime * mousedelta[1];
		r_arr[1]+= speed * deltaTime * mousedelta[0];
		position[0]=xpos;
		position[1]=ypos;
	}

	glfwSetScrollCallback(window,scroll);
	glm::mat4 transform = glm::mat4(1.0);
	if(!invert)
	{
		transform = glm::rotate(transform,r_arr[0],glm::vec3(1.0,0.0,0.0));
		transform = glm::rotate(transform,r_arr[1],glm::vec3(0.0,1.0,0.0));
	}
	else
	{
		transform = glm::rotate(transform,r_arr[0],glm::vec3(-1.0,0.0,0.0));
		transform = glm::rotate(transform,r_arr[1],glm::vec3(0.0,-1.0,0.0));
	}
	
	//transform	 = glm::scale(transform,glm::vec3(s_arr[0],s_arr[1],s_arr[2]));
	MyModelMatrix = transform;
	//printf(to_string(ModelMatrix).c_str());
	//printf("\n");
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}