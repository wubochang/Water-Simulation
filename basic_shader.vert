#version 330 core
//#extension GL_EXT_gpu_shader4 : enable
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNorm;

out float needRender;
out vec2 TexCoord;
out vec4 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_front;
uniform vec4 ClipPlane;
uniform mat4 invWaterModel;
uniform sampler2D heightmapTex;

void main()
{
	if( dot(camera_front, aNorm) > 0) 
	{
		needRender = 0.0f;
	}
	else
	{
		needRender = 1.0f;
	}
	if(aNorm.y == 1.0f) needRender = 1.0f;

	worldPos = model * vec4(aPos, 1.0f);

	gl_ClipDistance[0] = dot(vec4(aPos,1),ClipPlane);

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}