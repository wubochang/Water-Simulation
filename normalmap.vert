#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 vsColor;

uniform mat4 waterModel;
uniform sampler2D heightmapTex;

void main()
{
	vec2 tmp = vec2( (aTexCoord.x * 2) - 1, (aTexCoord.y * 2) - 1);

	float cur_height	= texelFetch(heightmapTex, ivec2(aPos.x, aPos.z), 0).r;
	float up_height		= texelFetch(heightmapTex, ivec2(aPos.x, aPos.z + 1), 0).r;
	float right_height	= texelFetch(heightmapTex, ivec2(aPos.x + 1, aPos.z), 0).r;

	vec4 cur_worldSpace = waterModel * vec4(aPos.x, aPos.y + 10 * cur_height, aPos.z, 1.0f);
	vec4 up_worldSpace = waterModel * vec4(aPos.x, aPos.y + 10 * up_height, aPos.z + 1, 1.0f);
	vec4 right_worldSpace = waterModel * vec4(aPos.x + 1, aPos.y + 10 * right_height, aPos.z, 1.0f);

	vec3 to_up = up_worldSpace.xyz - cur_worldSpace.xyz ;
	vec3 to_right = right_worldSpace.xyz - cur_worldSpace.xyz ;

	vec3 thisNorm = cross(to_up, to_right); 
	thisNorm = normalize(thisNorm);

	//if(abs(cur_height - up_height )< 1e-5 && abs(cur_height - right_height) < 1e-5)
		//vsColor = vec3(0.75, 0.25, 0.25);
	vsColor = thisNorm;

	gl_Position = vec4( tmp.x, tmp.y, 0.0f, 1.0f);
}