#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out float vsColor;

uniform sampler2D prevHeight;

void main()
{
	vec2 tmp = vec2( (aTexCoord.x * 2) - 1, (aTexCoord.y * 2) - 1);
	vsColor = texture(prevHeight, aTexCoord).x;
	gl_Position = vec4( tmp.x, tmp.y, 0.0f, 1.0f);
}