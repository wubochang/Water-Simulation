#version 330 core

in float vsColor;

out vec4 FragColor;

uniform sampler2D causticTexture;
uniform sampler2D heightmapTexture;
uniform sampler2D normalmapTexture;
uniform sampler2D depthTexture;


void main()
{
	FragColor = vec4(1.0f, vsColor, 0.0f, 0.5f);
}