#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNorm;

out float needRender;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_front;

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
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}