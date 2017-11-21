#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out float vsColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightmapTex;

void main()
{
	vsColor = texture(heightmapTex, aTexCoord).r;
	vec3 tmp = vec3(aPos.x, aPos.y + vsColor * 10, aPos.z);
	gl_Position = projection * view * model * vec4(tmp, 1.0f);
}