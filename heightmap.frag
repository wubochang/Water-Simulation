#version 330 core

in float vsColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(vsColor, 1.0f, 1.0f, 1.0f);
}