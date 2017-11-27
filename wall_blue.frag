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
		//vec2 WaterTexCoord = vec2(-1,-1);
		float t = (0.2 - worldPos.y) / light.y;
		//if(t >= 0) 
		vec2	WaterTexCoord = worldPos.xz + t * (0.75 * light.xz) + 0.5f;
		
		vec4 pos_waterspace_frag = invWaterModel * worldPos;
		float x_frag = pos_waterspace_frag.x / 128.0f;
		float z_frag = pos_waterspace_frag.z / 128.0f;
		float height = texture2D(heightmapTex, vec2(x_frag, z_frag)).r * 10;
		if(height >= pos_waterspace_frag.y)
		{
			FragColor = vec4(18.0f / 256.0f, 118.0f / 256.0f, 156.0f / 256.0f, 0.5f);
			float causticIntensity = texture(textureCaustic, WaterTexCoord).r;
			if(causticIntensity > 0)
				causticIntensity = causticIntensity * 2 - 0.035;
			FragColor += vec4(causticIntensity * vec3(1,1,1), 0);
			float shadowIntensity = min(max(min(0.5f - abs(0.5f - WaterTexCoord.x), 0.01), -0.01f), max(min(0.5f - abs(0.5f - WaterTexCoord.y), 0.01), -0.01f)) + 0.01f;
			FragColor += vec4(shadowIntensity / 0.01f * vec3(30.0f/256.0f, 30.0f/256.0f, 30.0f/256.0f), 0);
			return;
		}
	}
	FragColor = vec4(0, 0, 0, 0);
}