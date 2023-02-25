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
in vec2 v_TextureCoords;

uniform float u_TilingFactor;
uniform sampler2D u_Texutre;

void main()
{
	color = texture(u_Texutre, v_TextureCoords);
	float average = (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b) / 3.0;
	color = vec4(average, average, average, 1.0);
}