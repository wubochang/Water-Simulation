#version 330 core

in vec3 oldPos;
in vec3 newPos;
in vec3 ray;

layout (location = 0) out vec4 caustic;


void main() {

	float area = length(dFdx(newPos) * dFdy(newPos));


	//float oldArea = length(dFdx(oldPos) * dFdy(oldPos));
	//float newArea = length(dFdx(newPos) * dFdy(newPos));
	//float area = length(dFdx(newPos) * dFdy(newPos) / dFdx(oldPos) / dFdy(oldPos));
	caustic = vec4(area, 1.0, 0.0, 1.0);
	//caustic = vec4(ray, 1.0);
}