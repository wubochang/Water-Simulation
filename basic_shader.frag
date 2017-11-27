#version 330 core
out vec4 FragColor;

in float needRender;
in vec2 TexCoord;
in vec4 worldPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 light;
uniform mat4 invWaterModel;
uniform sampler2D heightmapTex;
uniform sampler2D textureCaustic;

void main()
{
	if(needRender > 0.0f) {
		FragColor = mix(texture(texture1, TexCoord) ,texture(texture2, TexCoord), 0.2);

		float t = (0.2 - worldPos.y) / light.y;
		float t2 = (0.5 - 0.2) / light.y;
		vec2 shadowCoord = worldPos.xz + t * (0.75 * light.xz) + t2 * (light.xz);
		float shadowBoundary = max(abs(shadowCoord.x), abs(shadowCoord.y));
		float shadowIntensity = max(min(shadowBoundary - 0.5, 0.02), -0.01) / 0.03;
		FragColor += vec4(shadowIntensity * vec3(-0.2, -0.2, -0.2), 0);
		
		vec4 pos_waterspace_frag = invWaterModel * worldPos;
		float x_frag = pos_waterspace_frag.x / 128.0f;
		float z_frag = pos_waterspace_frag.z / 128.0f;
		float height = texture2D(heightmapTex, vec2(x_frag, z_frag)).r * 10;
		if(height >= pos_waterspace_frag.y)
		{
			FragColor *= vec4(0.3, 0.3, 0.3, 1);
			FragColor += vec4(18.0f / 256.0f, 118.0f / 256.0f, 156.0f / 256.0f, 0.0f);
			float causticIntensity = texture(textureCaustic, shadowCoord + 0.5).r;
			if(causticIntensity > 0)
				causticIntensity = causticIntensity * 1.3 - 0.006;
			FragColor += vec4(causticIntensity * vec3(1,1,1), 0);
			return;
		}
	}
	else {
		FragColor = vec4(0, 0, 0, 0);
	}
}