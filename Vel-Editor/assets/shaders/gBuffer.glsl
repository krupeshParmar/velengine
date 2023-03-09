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
} vs_out;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Transform;
uniform mat4 u_InverseTransform;

void main()
{
	vs_out.TexCoords = vec2(vUVx2.x, -vUVx2.y);
	vs_out.FragPos = u_Transform * vec4(vPosition.xyz, 1.0);
	vs_out.Normal.xyz = normalize(u_InverseTransform * vec4(vNormal.xyz, 1.0f)).xyz;
	vs_out.Normal.w = 1.0f;
	gl_Position = u_Projection * u_View * u_Transform * vec4(vPosition.xyz, 1.0);
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
	bool useBloom = false;
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
		useBloom = true;
	}

	vec4 specularColour = SPEC;
	if (useTextureSpecular)
		specularColour = texture(u_TextureSpecular, fs_in.TexCoords);

	vec4 emissiveColour = EMIS;
	if(useTextureEmissive)
		emissiveColour = texture(u_TextureEmissive, fs_in.TexCoords);

	f_albedo.rgb = textColour0.rgb;
	f_albedo.a = transparency;
	f_position = fs_in.FragPos;
	f_normal = vec4(normalValue.xyz, 1.0);
	f_specular = specularColour;
	f_emissive = emissiveColour * EmissiveIntensity;

	float ratio = 1.00 / 1.52;
	vec3 I = normalize(fs_in.FragPos.xyz - eyeLocation.xyz);

	vec3 RR = refract(I, normalValue.xyz, ratio);
	vec3 RL = reflect(I, normalValue.xyz);
	f_albedo.rgb += texture(skyBox, RR).xyz * SHIN;
	f_albedo.rgb += texture(skyBox, RL).xyz * SHIN;
	f_bloom = vec4(0.0, 0.0, 0.0, 0.0);

	//if (useBloom)
	{
		/*if (f_albedo.r > 0.05f)
			f_albedo.r *= 2.0f;

		if (f_albedo.g > 0.05f)
			f_albedo.g *= 2.0f;

		if (f_albedo.b > 0.05f)
			f_albedo.b *= 2.0f;

		float brightness = dot(f_albedo.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
		if (brightness > 0.15f)
			f_bloom = vec4(f_albedo.rgb, 1.0);*/
	}
}
