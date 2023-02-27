#type vertex
#version 330 core

//layout(location = 0) in vec2 a_TextureCoords;
//layout(location = 1) in vec4 a_Position;

layout(location = 0)in vec4 vColour;			// Was vec3
layout(location = 1)in vec4 vPosition;			// Was vec3
layout(location = 2)in vec4 vNormal;			// Vertex normal
layout(location = 3)in vec4 vUVx2;				// 2 x Texture coords
layout(location = 4)in vec4 vTangent;			// For bump mapping
layout(location = 5)in vec4 vBiNormal;			// For bump mapping
layout(location = 6)in vec4 vBoneID;			// For skinned mesh (FBX)
layout(location = 7)in vec4 vBoneWeight;		// For skinned mesh (FBX)

out vec2 v_TextureCoords;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;


void main()
{
	v_TextureCoords = vec2(vUVx2.x, vUVx2.y);
	gl_Position = u_ViewProjection * u_Transform * vPosition;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_TextureCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    int type;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

uniform Material material;

uniform float u_TilingFactor;
uniform sampler2D u_Texutre;

void main()
{

	color = texture(u_Texutre, v_TextureCoords + vec2(u_TilingFactor.x, 1.0));
}