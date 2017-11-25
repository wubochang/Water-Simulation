#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 oldPos;
out vec3 newPos;
out vec3 ray;

uniform vec3 light;
uniform sampler2D heightmapTex;
uniform sampler2D normalmapTex;

const float IOR_AIR = 1.0;
const float IOR_WATER = 1.333;


/* get the two points where the line (pt+t*dir) intersects with the cube */
vec2 intersectWithCube(vec3 pt, vec3 dir, vec3 cubeMin, vec3 cubeMax) {
	vec3 tMin = (cubeMin - pt) / dir;
	vec3 tMax = (cubeMax - pt) / dir;
	vec3 tRealMin = min(tMin, tMax);
	vec3 tRealMax = max(tMin, tMax);
	float tNear = max(max(tRealMin.x, tRealMin.y), tRealMin.z);
	float tFar = min(min(tRealMax.x, tRealMax.y), tRealMax.z);

	return vec2(tNear, tFar);
}

/* project the point pt along direction dir onto the cube */
vec3 project(vec3 pt, vec3 dir) {
	vec2 t = intersectWithCube(pt, dir, vec3(-0.5f, -0.5f, -0.5f), vec3(0.5f, 0.5f, 0.5f));
	pt += dir * t.y;
	return pt;
}

void main() {
	vec3 normal = texture(normalmapTex, aTexCoord).xyz;
	float height = texture(heightmapTex, aTexCoord).r;

	vec3 refractedLight = refract(-light, vec3(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
    ray = refract(-light, normal, IOR_AIR / IOR_WATER);
    oldPos = project(aPos, refractedLight);
    newPos = project(aPos + vec3(0.0, height * 10.0f, 0.0), ray);
	//ray = normal;

	gl_Position = vec4(aTexCoord * 2 - 1, 0.0f, 1.0f);
}