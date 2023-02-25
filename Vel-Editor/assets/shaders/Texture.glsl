#type vertex
#version 330 core

layout(location = 0) in vec2 a_TextureCoords;
layout(location = 1) in vec4 a_Position;

out vec2 v_TextureCoords;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
	v_TextureCoords = vec2(a_TextureCoords.x, a_TextureCoords.y);
	gl_Position = u_ViewProjection * u_Transform * a_Position;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TextureCoords;

uniform float u_TilingFactor;
uniform sampler2D u_Texutre;

void main()
{
	color = texture(u_Texutre, v_TextureCoords + vec2(u_TilingFactor.x, 1.0));
}