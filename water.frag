#version 330 core

in VS_OUT {
	vec2 TexCoord;
	vec4 vsColor;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPos_screen;
} fs_in;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D RflTexture;
uniform sampler2D RfrTexture;

void main()
{
	vec3 cameraToFrag = normalize(viewPos - fs_in.FragPos);
	float rflFactor = dot(cameraToFrag, normalize(lightPos - fs_in.FragPos));
	rflFactor = clamp(rflFactor, 0, 1);
	//rflFactor = pow(rflFactor, 0.5);

	vec2 ndc = (fs_in.FragPos_screen.xy / fs_in.FragPos_screen.w ) / 2.0 + 0.5;
	float deltaU, deltaV;
	float factor = 0.1;
	deltaU = -factor * fs_in.Normal.x;
	deltaV = -factor * fs_in.Normal.z;
	vec2 RflCoord = vec2(1-ndc.x + deltaU, ndc.y + deltaV);
	vec2 RfrCoord = vec2(ndc.x + deltaU, ndc.y + deltaV);

	RflCoord = clamp(RflCoord, 0, 1);
	RfrCoord = clamp(RfrCoord, 0, 1);

	vec4 RflColor = texture(RflTexture, RflCoord);
	vec4 RfrColor = texture(RfrTexture, RfrCoord);

	//vec4 mixRlRrColor = RfrColor;
	vec4 mixRlRrColor = mix(RflColor, RfrColor, rflFactor);

	vec3 color = vec3(78.0f / 256.0f, 168.0f / 256.0f, 216.0f / 256.0f);
	mixRlRrColor = mix(mixRlRrColor, vec4(color,1.0f), 0.3);

	// Ambient
    vec3 ambient = 0.25 * mixRlRrColor.xyz;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * mixRlRrColor.xyz;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}