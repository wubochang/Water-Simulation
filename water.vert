#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out VS_OUT {
	vec2 TexCoord;
	vec4 vsColor;
	vec3 FragPos;
	vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightmapTex;
uniform sampler2D normalmapTex;

void main()
{
	float height = texture(heightmapTex, aTexCoord).r;
	vec3 tmp = vec3(aPos.x, aPos.y + height * 10, aPos.z);
	vs_out.vsColor = texture(normalmapTex, aTexCoord);
	vs_out.TexCoord = aTexCoord;
	vs_out.FragPos = (model * vec4(tmp, 1.0f) ).xyz;
	vs_out.Normal = texture(normalmapTex, aTexCoord).xyz;
	gl_Position = projection * view * model * vec4(tmp, 1.0f);
}