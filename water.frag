#version 330 core

in vec2 TexCoord;
in vec4 vsColor;

out vec4 FragColor;


void main()
{
	//FragColor = texture(textureRenderTex, TexCoord);
	//FragColor = vec4(1.0f, vsColor, 0.0f, 1.0f);
	FragColor = vsColor;
}