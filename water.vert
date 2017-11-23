#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 vsColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightmapTex;
uniform sampler2D textureRenderTex;

void main()
{
	float height = texture(heightmapTex, aTexCoord).r;
	vec3 tmp = vec3(aPos.x, aPos.y + height * 10, aPos.z);
	vsColor = texture(textureRenderTex, aTexCoord);
	TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(tmp, 1.0f);
}