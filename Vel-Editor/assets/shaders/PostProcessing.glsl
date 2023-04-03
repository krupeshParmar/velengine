#type vertex
#version 420 core

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
#version 420 core

layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 f_position;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_specular;
layout(location = 4) out vec4 f_emissive;
layout(location = 5) out vec4 f_bloom;
in vec2 v_TextureCoords;

uniform float u_TilingFactor;
uniform sampler2D gAlbedo;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gSpecular;
uniform sampler2D gBloom;
uniform sampler2D gNoise;
uniform vec4 eyeLocation;
uniform vec2 screen_width_height;					// x = width, y = height
uniform float Exposure;

uniform bool DepthOfField;
uniform float FocalDistance;

uniform bool UseBloom;
uniform float BloomIntensity;
uniform float weights1[5] = float[](0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

uniform float time; // time in seconds
uniform float timeInc;
uniform bool nightVision;
uniform bool rippleEffect;
uniform bool grayscale;
float hash(in float n) { return fract(sin(n) * 43758.5453123); }

const int M = 16;
const int N = 2 * M + 1;

uniform float weights2[N] = float[](
	0.012318109844189502,
	0.014381474814203989,
	0.016623532195728208,
	0.019024086115486723,
	0.02155484948872149,
	0.02417948052890078,
	0.02685404941667096,
	0.0295279624870386,
	0.03214534135442581,
	0.03464682117793548,
	0.0369716985390341,
	0.039060328279673276,
	0.040856643282313365,
	0.04231065439216247,
	0.043380781642569775,
	0.044035873841196206,
	0.04425662519949865,
	0.044035873841196206,
	0.043380781642569775,
	0.04231065439216247,
	0.040856643282313365,
	0.039060328279673276,
	0.0369716985390341,
	0.03464682117793548,
	0.03214534135442581,
	0.0295279624870386,
	0.02685404941667096,
	0.02417948052890078,
	0.02155484948872149,
	0.019024086115486723,
	0.016623532195728208,
	0.014381474814203989,
	0.012318109844189502
	);



float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.05f, float offset = 5.0f)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * zVal - offset)));
}

vec4 vignette(vec4 at, vec4 pos)
{
	float dx = 1.3 * abs(pos.x - .5);
	float dy = 1.3 * abs(pos.y - .5);
	return at * (1.0 - dx * dx - dy * dy);
}

vec4 gammaf(vec4 x, float f)
{
	return pow(x, vec4(1. / f));
}

float random2D(vec2 coord)
{
	return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 blurImage(sampler2D text, int iterations, int whichWeights)
{
	vec3 result = vec3(0.0f, 0.f, 0.f);
	vec2 tex_offset = 1.0f / textureSize(text, 0);

	for (int k = 0; k < iterations; k++)
	{
		if (whichWeights == 0)
		{
			for (int i = 0; i < N; ++i)
			{
				//result += texture(gBloom, v_TextureCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights2[i];
				//result += texture(gBloom, v_TextureCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights2[i];
				vec2 tc = v_TextureCoords + tex_offset * vec2(float(i - M), 0);
				result += weights2[i] * texture(text, tc).rgb;
			}

			for (int j = 0; j < N; ++j)
			{
				//result += texture(gBloom, v_TextureCoords + vec2(0.f, tex_offset.y * j)).rgb * weights2[j];
				//result += texture(gBloom, v_TextureCoords - vec2(0.f, tex_offset.y * j)).rgb * weights2[j];
				vec2 tc = v_TextureCoords + tex_offset * vec2(0, float(j - M));
				result += weights2[j] * texture(text, tc).rgb;
			}
		}
		else
		{
			for (int i = 0; i < N; ++i)
			{
				//result += texture(gBloom, v_TextureCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights2[i];
				//result += texture(gBloom, v_TextureCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights2[i];
				vec2 tc = v_TextureCoords + tex_offset * vec2(float(i - M), float(i - M));
				result += weights2[i] * texture(text, tc).rgb;
			}

			//for (int j = 0; j < 5; ++j)
			//{
			//	//result += texture(gBloom, v_TextureCoords + vec2(0.f, tex_offset.y * j)).rgb * weights2[j];
			//	//result += texture(gBloom, v_TextureCoords - vec2(0.f, tex_offset.y * j)).rgb * weights2[j];
			//	vec2 tc = v_TextureCoords + tex_offset * vec2(0, float(j - 2));
			//	result += weights2[j] * texture(text, tc).rgb;
			//}
		}
	}
	return result;
}


void main()
{
	vec3 FragPos = texture(gPosition, v_TextureCoords).rgb;
	vec4 Normal = texture(gNormal, v_TextureCoords);
	vec3 Diffuse = texture(gAlbedo, v_TextureCoords).rgb;
	vec4 Specular = texture(gSpecular, v_TextureCoords);

	f_color = vec4(Diffuse.rgb, 1.0);

	/// DOF
	if (DepthOfField)
	{
		if (distance(eyeLocation.rgb, FragPos) > FocalDistance)
			f_color = vec4(blurImage(gAlbedo, 1, 1), 1.f);
	}

	if (UseBloom)
	{
		f_bloom = vec4(blurImage(gBloom, 10, 0), 1.0f);

		f_color += vec4(f_bloom.rgb, 1.0) * BloomIntensity;
	}

	f_position = vec4(FragPos, 1.0);


	///// BLOOM 
	//vec3 result = texture(gBloom, v_TextureCoords).rgb * weights[0];
	//for (int i = 1; i < 5; i++)
	//{
	//	for (int j = 1; j < 5; j++)
	//	{
	//		result += texture(gBloom, v_TextureCoords + vec2(tex_offset.x * i, tex_offset.y * i)).rgb * weights[i];
	//		result += texture(gBloom, v_TextureCoords - vec2(tex_offset.x * i, tex_offset.y * i)).rgb * weights[i];
	//	}
	//}

	//// NIGHT VISION
	if (nightVision)
	{
		float v = max(max(Diffuse.r, Diffuse.g), Diffuse.b);
		f_color = vec4(0.f, v, 0.f, 1.0);
		float noise = (random2D(v_TextureCoords + time) - 0.5) * 0.6;
		//float noise = noisef(1.0);
		f_color.g += noise;
		f_color.r += noise;
		f_color.b += noise;
	}

	/// Ripple
	if (rippleEffect)
	{
		vec2 tc = v_TextureCoords.xy;
		vec2 p = -1.0 + 2.0 * tc;
		float len = length(p);
		vec2 uv = tc + (p / len) * cos(len * 12.0 - timeInc * 4.0) * 0.03;
		vec3 col = texture2D(gAlbedo, uv).xyz;
		f_color = vec4(col, 1.0);
	}


	/// Grayscale
	if (grayscale)
	{
		float average = (0.2126 * f_color.r + 0.7152 * f_color.g + 0.0722 * f_color.b) / 3.0;
		f_color = vec4(average, average, average, 1.0);
	}
}