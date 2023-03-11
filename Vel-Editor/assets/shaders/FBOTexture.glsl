#type vertex
#version 330 core

layout(location = 0) in vec2 a_TextureCoords;
layout(location = 1) in vec4 a_Position;

out vec2 v_TextureCoords;
uniform mat4 u_Transform;

void main()
{
	v_TextureCoords = vec2(a_TextureCoords.x, a_TextureCoords.y);
	gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 pos;
layout(location = 2) out vec4 norm;
layout(location = 3) out vec4 spec;
in vec2 v_TextureCoords;

uniform float u_TilingFactor;
uniform sampler2D gAlbedoSpec;


uniform vec2 resolution; // Screen resolution
uniform float time; // time in seconds
uniform bool rippleEffect;
uniform bool nightVision;


float hash(in float n) { return fract(sin(n) * 43758.5453123); }

float random2D(vec2 coord)
{
	return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
void main()
{
	color = texture(gAlbedoSpec, v_TextureCoords);
	spec = vec4(1.0, 1.0, 1.0, 1.0);

    ///// RIPPLE EFFECT
	if (rippleEffect)
	{
		vec2 tc = v_TextureCoords.xy;
		vec2 p = -1.0 + 2.0 * tc;
		float len = length(p);
		vec2 uv = tc + (p / len) * cos(len * 12.0 - time * 4.0) * 0.03;
		vec3 col = texture2D(gAlbedoSpec, uv).xyz;
		color = vec4(col, 1.0);
	}

    //// NIGHT VISION
	if (nightVision)
	{
		float v = max(max(color.r, color.g), color.b);
		color = vec4(0.f, v, 0.f, 1.0);
		float noise = (random2D(v_TextureCoords + time) - 0.5) * 0.6;
		//float noise = noisef(1.0);
		color.g += noise;
		color.r += noise;
		color.b += noise;
	}


	/*float average = (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b) / 3.0;
	color = vec4(average, average, average, 1.0);*/
}