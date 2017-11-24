#version 330 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.Normal = aPos;
	vs_out.FragPos = (model * vec4(aPos, 1.0f)).xyz;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}