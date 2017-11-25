#version 330 core
out vec4 FragColor;

in float needRender;
in vec2 TexCoord;
in vec4 worldPos;

uniform vec3 light;
uniform mat4 invWaterModel;
uniform sampler2D heightmapTex;
uniform sampler2D textureCaustic;

void main()
{
	if(needRender > 0.0f)
	{
		vec2 WaterTexCoord = vec2(-1,-1);
		float t = (0.2 - worldPos.y) / light.y;
		if(t >= 0) 
			WaterTexCoord = worldPos.xz + t * (0.75 * light.xz) + 0.5f;
		
		vec4 pos_waterspace_frag = invWaterModel * worldPos;
		float x_frag = pos_waterspace_frag.x / 128.0f;
		float z_frag = pos_waterspace_frag.z / 128.0f;
		float height = texture2D(heightmapTex, vec2(x_frag, z_frag)).r * 10;
		if(height >= pos_waterspace_frag.y)
		{
			FragColor = vec4(78.0f / 256.0f, 168.0f / 256.0f, 216.0f / 256.0f, 0.5f);
			FragColor += vec4(texture(textureCaustic, WaterTexCoord).r * 1.5 * vec3(1,1,1), 0);
			//FragColor += vec4(texture(textureCaustic, WaterTexCoord).g * vec3(0.1,0.1,0.1), 0);

			return;
		}
	}
	FragColor = vec4(0, 0, 0, 0);
}