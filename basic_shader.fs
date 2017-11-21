#version 330 core
out vec4 FragColor;

in float needRender;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	if(needRender > 0.0f)
		FragColor = mix(texture(texture1, TexCoord) ,texture(texture2, TexCoord), 0.2);
	else
		FragColor = vec4(0, 0, 0, 0);
}