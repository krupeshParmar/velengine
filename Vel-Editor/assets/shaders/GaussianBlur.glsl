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

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 f_position;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_specular;
layout(location = 4) out vec4 f_bloom;
in vec2 v_TextureCoords;
uniform sampler2D screenTexture;
uniform bool horizontal;
uniform float weights[5] = float[](0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

void main()
{
	vec2 tex_offset = 1.0f / textureSize(screenTexture, 0);
	vec3 result = texture(screenTexture, v_TextureCoords).rgb * weights[0];
	if (horizontal)
	{
		for (int i = 1; i < 5; i++)
		{
			result += texture(screenTexture, v_TextureCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
			result += texture(screenTexture, v_TextureCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; i++)
		{
			result += texture(screenTexture, v_TextureCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
			result += texture(screenTexture, v_TextureCoords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
		}
	}
	f_bloom = vec4(result, 1.0f);
}