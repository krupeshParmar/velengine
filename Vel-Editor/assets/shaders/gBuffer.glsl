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
	vec4 FragPos;
	vec4 Normal;
	vec2 TexCoords;
	vec4 vColour;
} vs_out;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Transform;
uniform mat4 u_InverseTransform;
uniform bool u_UseBone;
const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform mat4 u_RotationMatrix[MAX_BONES];
//assets/models/maria/Maria WProp J J Ong@Two Handed Sword Death.fbx
//assets/models/maria/Maria WProp J J Ong@Great Sword Idle.fbx
void main()
{
	vs_out.TexCoords = vec2(vUVx2.x, -vUVx2.y);
	vec4 originalPosition = vec4(vPosition.xyz, 1.f);
	vec4 totalPosition = originalPosition;
	vec4 normal = vec4(0.f);
	normal.xyz = normalize(u_InverseTransform * vec4(vNormal.xyz, 1.f)).xyz;
	normal.w = 1.f;
	vs_out.vColour = vec4(1.f, 1.f, 1.f,1.f);
	if (u_UseBone)
	{
		totalPosition = vec4(0.f);
		normal.xyz = vec3(0.f);
		int boneId0 = int(vBoneID[0]);
		int boneId1 = int(vBoneID[1]);
		int boneId2 = int(vBoneID[2]);
		int boneId3 = int(vBoneID[3]);
		vs_out.vColour = vec4(1.f, 0.f, 1.f,1.f);
		mat4 boneTransform = finalBonesMatrices[boneId0] * vBoneWeight[0];
		boneTransform += finalBonesMatrices[boneId1] * vBoneWeight[1];
		boneTransform += finalBonesMatrices[boneId2] * vBoneWeight[2];
		boneTransform += finalBonesMatrices[boneId3] * vBoneWeight[3];
		totalPosition = boneTransform * originalPosition;
		//totalPosition.w = 1.f;
		normal.xyz = mat3(finalBonesMatrices[boneId0]) * normalize(vNormal.xyz) * vBoneWeight[0];
		normal.xyz += mat3(finalBonesMatrices[boneId1]) * normalize(vNormal.xyz) * vBoneWeight[1];
		normal.xyz += mat3(finalBonesMatrices[boneId2]) * normalize(vNormal.xyz) * vBoneWeight[2];
		normal.xyz += mat3(finalBonesMatrices[boneId3]) * normalize(vNormal.xyz) * vBoneWeight[3];

		// for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		// {
		// 	if (vBoneID[i] == -1)
		// 	{
		// 		vs_out.vColour = vec4(0.f, 0.f, 1.f,1.f);
		// 		continue;
		// 	}
		// 	if (vBoneID[i] > MAX_BONES)
		// 	{
		// 		totalPosition = originalPosition;
		// 		vs_out.vColour = vec4(1.f, 1.f, 0.f, 1.f);
		// 		normal.xyz = normalize(u_InverseTransform * vec4(vNormal.xyz, 1.0f)).xyz;
		// 		break;
		// 	}
		// 	if(vBoneID[i] == 0)
		// 		vs_out.vColour = vec4(0.f, 1.f, 0.f,1.f);
		// 	else
		// 		vs_out.vColour = vec4(1.f, 0.f, 1.f,1.f);
		// 	vec4 localPosition = finalBonesMatrices[vBoneID[i]] * originalPosition;
		// 	totalPosition += localPosition * vBoneWeight[i];
		// 	normal.xyz += mat3(finalBonesMatrices[vBoneID[i]]) * normalize(vNormal.xyz) * vBoneWeight[i];

		// 	normal.xyz = (u_RotationMatrix[vBoneID[i]] * normalize(normal)).xyz;
		// }
		normal.w = 1.f;
	}
	vs_out.FragPos = u_Transform * totalPosition;
	vs_out.Normal = normalize(normal);
	gl_Position = u_Projection * u_View * u_Transform * totalPosition;
}

#type fragment
#version 330 core
layout(location = 0) out vec4 f_albedo;
layout(location = 1) out vec4 f_position;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_specular;
layout(location = 4) out vec4 f_emissive;
layout(location = 5) out vec4 f_bloom;

in VS_OUT
{
	vec4 FragPos;
	vec4 Normal;
	vec2 TexCoords;
	vec4 vColour;
} fs_in;


uniform sampler2D u_TextureDiffuse;
uniform sampler2D u_TextureNormal;
uniform sampler2D u_TextureSpecular;
uniform sampler2D u_TextureEmissive;
uniform bool useTextureDiffuse;
uniform bool useTextureNormal;
uniform bool useTextureSpecular;
uniform bool useTextureEmissive;
uniform vec4 SPEC;
uniform vec4 RGBA;
uniform vec4 EMIS;
uniform samplerCube skyBox;
uniform float SHIN;
uniform float EmissiveIntensity;
uniform float u_texsize;
uniform vec4 eyeLocation;


void main()
{
	vec4 normalValue = normalize(fs_in.Normal);
	if (useTextureNormal)
	{
		normalValue = texture(u_TextureNormal, fs_in.TexCoords);
		normalValue = normalize(normalValue * 2.0 - 1.0);
	}

	vec3 textColour0 = texture(u_TextureDiffuse, fs_in.TexCoords * u_texsize).rgb;
	float transparency = texture(u_TextureDiffuse, fs_in.TexCoords).w;
	if (!useTextureDiffuse)
	{
		textColour0 = RGBA.rgb;
		transparency = RGBA.a;
	}

	vec4 specularColour = SPEC;
	if (useTextureSpecular)
		specularColour = texture(u_TextureSpecular, fs_in.TexCoords);

	vec4 emissiveColour = EMIS;
	if(useTextureEmissive)
		emissiveColour = texture(u_TextureEmissive, fs_in.TexCoords);

	f_albedo.rgb = textColour0.rgb;
	//f_albedo.rgb = fs_in.vColour.rgb;
	f_albedo.a = transparency;
	f_position = fs_in.FragPos;
	f_normal = vec4(normalValue.xyz, 1.0);
	f_specular = specularColour;
	f_emissive = emissiveColour * EmissiveIntensity;
	f_emissive.a = 1.f;

	float ratio = 1.00 / 1.52;
	vec3 I = normalize(fs_in.FragPos.xyz - eyeLocation.xyz);

	vec3 RR = refract(I, normalValue.xyz, ratio);
	vec3 RL = reflect(I, normalValue.xyz);
	f_albedo.rgb += texture(skyBox, RR).xyz * SHIN;
	f_albedo.rgb += texture(skyBox, RL).xyz * SHIN;
	f_bloom = vec4(0.0, 0.0, 0.0, 0.0);
}
