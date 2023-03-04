#type vertex
#version 330 core
layout(location = 0)in vec4 vColour;			// Was vec3
layout(location = 1)in vec4 vPosition;			// Was vec3
layout(location = 2)in vec4 vNormal;			// Vertex normal
layout(location = 3)in vec4 vUVx2;				// 2 x Texture coords
layout(location = 4)in vec4 vTangent;			// For bump mapping
layout(location = 5)in vec4 vBiNormal;			// For bump mapping
layout(location = 6)in vec4 vBoneID;			// For skinned mesh (FBX)
layout(location = 7)in vec4 vBoneWeight;		// For skinned mesh (FBX)

out VS_OUT
{
	vec3 FragPos;
	vec4 Normal;
	vec2 TexCoords;
} vs_out;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Transform;
uniform mat4 u_InverseTransform;

void main()
{
	vs_out.TexCoords = vec2(vUVx2.x, -vUVx2.y);
	vs_out.FragPos = (u_Transform * vec4(vPosition.xyz, 1.0f)).xyz;
	vs_out.Normal = normalize(u_InverseTransform * vNormal);
	gl_Position = u_Projection * u_View * u_Transform * vec4(vPosition.xyz, 1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 f_albedo;
layout(location = 1) out vec4 f_position;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_specular;

in VS_OUT
{
	vec3 FragPos;
	vec4 Normal;
	vec2 TexCoords;
} fs_in;


uniform sampler2D u_TextureDiffuse;
uniform sampler2D u_TextureNormal;
uniform sampler2D u_TextureSpecular;
uniform bool useTextureDiffuse;
uniform bool useTextureNormal;
uniform bool useTextureSpecular;
uniform vec4 SPEC;
uniform vec4 RGBA;
uniform samplerCube skyBox;
uniform float SHIN;
uniform vec4 eyeLocation;

void main()
{
	vec3 normalValue = normalize(fs_in.Normal.xyz);
	if (useTextureNormal)
	{
		normalValue = texture(u_TextureNormal, fs_in.TexCoords).rgb;
		normalValue = normalize(normalValue * 2.0 - 1.0);
	}

	vec3 textColour0 = texture(u_TextureDiffuse, fs_in.TexCoords).rgb;
	float transparency = texture(u_TextureDiffuse, fs_in.TexCoords).w;
	if (!useTextureDiffuse)
	{
		textColour0 = RGBA.rgb;
		transparency = RGBA.a;
	}

	vec4 specularColour = SPEC;
	if (useTextureSpecular)
		specularColour = texture(u_TextureSpecular, fs_in.TexCoords);

	f_albedo.rgb = textColour0.rgb;
	f_albedo.a = transparency;
	f_position = vec4(fs_in.FragPos, 1.0);
	f_normal = vec4(normalValue, 1.0);
	f_specular = specularColour;

	float ratio = 1.00 / 1.52;
	vec3 I = normalize(fs_in.FragPos - eyeLocation.xyz);

	vec3 RR = refract(I, normalValue, ratio);
	vec3 RL = reflect(I, normalValue);
	f_albedo.rgb += texture(skyBox, RR).xyz * SHIN;
	f_albedo.rgb += texture(skyBox, RL).xyz * SHIN;
}
