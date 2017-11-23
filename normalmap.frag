#version 330 core

in vec3 vsColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(vsColor, 1.0f);
}