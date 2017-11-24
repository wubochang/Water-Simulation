#version 330 core
out vec4 FragColor;

in float needRender;
in vec2 TexCoord;
in vec4 worldPos;

uniform mat4 invWaterModel;
uniform sampler2D heightmapTex;

void main()
{
	if(needRender > 0.0f)
	{
		vec4 pos_waterspace_frag = invWaterModel * worldPos;
		float x_frag = pos_waterspace_frag.x / 128.0f;
		float z_frag = pos_waterspace_frag.z / 128.0f;
		float height = texture2D(heightmapTex, vec2(x_frag, z_frag)).r * 10;
		if(height >= pos_waterspace_frag.y)
		{
			FragColor = vec4(78.0f / 256.0f, 168.0f / 256.0f, 216.0f / 256.0f, 0.5f);
			return;
		}
	}
	FragColor = vec4(0, 0, 0, 0);
}