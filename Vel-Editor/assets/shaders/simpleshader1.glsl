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
    vec3 Normal;
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
    vs_out.Normal = normalize(u_InverseTransform * vec4(vNormal.xyz, 1.0f)).xyz;
    gl_Position = u_Projection * u_View * u_Transform * vec4(vPosition.xyz, 1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 f_color;
layout(location = 1) out vec4 f_position;
layout(location = 2) out vec4 f_normal;
layout(location = 3) out vec4 f_specular;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


uniform sampler2D u_TextureDiffuse;
uniform sampler2D u_TextureNormal;
uniform sampler2D u_TextureSpecular;
uniform bool useTextureDiffuse;
uniform bool useTextureNormal;
uniform bool useTextureSpecular;
uniform vec4 eyeLocation; 
uniform vec4 SPEC;
uniform vec4 RGBA;

struct Light
{
	vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 atten;     // x = constant, y = linear, z = quadratic, w = DistanceCutOff
    vec4 direction;	// Spot, directional lights
    vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = On or Off
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 20;
Light theLights[NUMBEROFLIGHTS];  	

vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal,
    vec3 vertexWorldPos, vec4 vertexSpecular);


void main()
{
	vec3 normalValue = fs_in.Normal.xyz;
	if (useTextureNormal)
		normalValue = texture(u_TextureNormal, fs_in.TexCoords).rgb;

	vec3 textColour0 = texture(u_TextureDiffuse, fs_in.TexCoords).rgb;
	float transparency = texture(u_TextureDiffuse, fs_in.TexCoords).w;
	if (!useTextureDiffuse)
		textColour0 = RGBA.rgb;

	vec4 specularColour = SPEC;
	if (useTextureSpecular)
		specularColour = texture(u_TextureSpecular, fs_in.TexCoords);

	////// TEMP FOR PROJECT
	theLights[0].position = vec4(0.0, 30.0, 0.0, 1.0);
	theLights[0].direction = vec4(0.0, 0.0, 0.0, 1.0);
	theLights[0].diffuse =	vec4(1.0, 1.0, 1.0, 1.0);
	theLights[0].specular = vec4(0.906863, 0.906863, 0.475658, 1.0);
	theLights[0].atten = vec4(0.075000, 0.049000, 0.0, 1.0);
	theLights[0].param1 = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 outColour = calculateLightContrib(textColour0.rgb, normalValue.xyz,
		fs_in.FragPos.xyz, specularColour);

	f_color = vec4(outColour.rgb, 1.0);

	float ambientLight = 0.3f;
	f_color.rgb += (textColour0.rgb * ambientLight);
	f_color.w = 1.0f;
	f_position = vec4(fs_in.FragPos, 1.0);
	f_normal = vec4(normalValue, 1.0);
	f_specular = specularColour;
	f_color = vec4(textColour0.rgb, 1.f);
}

vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal,
	vec3 vertexWorldPos, vec4 vertexSpecular)
{
	vec3 norm = normalize(vertexNormal);

	vec4 finalObjectColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	for (int index = 0; index < NUMBEROFLIGHTS; index++)
	{
		// ********************************************************
		// is light "on"
		if (theLights[index].param1.w == 0.0f)
		{	// it's off
			continue;
		}

		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);

		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if (intLightType == DIRECTIONAL_LIGHT_TYPE)		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;

			// Get the dot product of the light and normalize
			float dotProduct = dot(-theLights[index].direction.xyz,
				normalize(norm.xyz));	// -1 to 1

			dotProduct = max(0.0f, dotProduct);		// 0 to 1

			lightContrib *= dotProduct;

			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib);
			//+ (materialSpecular.rgb * lightSpecularContrib.rgb);
// NOTE: There isn't any attenuation, like with sunlight.
// (This is part of the reason directional lights are fast to calculate)

// return finalObjectColour;		

// Go to next light (skip rest)
			continue;
		}

		// Assume it's a point light 
		// intLightType = 0

		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);

		dotProduct = max(0.0f, dotProduct);

		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;


		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);

		vec3 reflectVector = reflect(-lightVector, normalize(norm.xyz));

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w;

		//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
		//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		// This only takes into account the colour of the light.
		//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
		//			                   * theLights[index].specular.rgb;

		// This one takes into account the object "colour" AND the light colour
		vec3 halfwayDir = normalize(lightVector + eyeLocation.xyz);
		float spec = pow(max(dot(norm, halfwayDir), 0.0), objectSpecularPower);

		lightSpecularContrib = spec
			* (vertexSpecular.rgb * theLights[index].specular.rgb);
		// Attenuation
		float attenuation = 1.0f /
			(theLights[index].atten.x +
				theLights[index].atten.y * distanceToLight +
				theLights[index].atten.z * distanceToLight * distanceToLight);

		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;


		// But is it a spot light
		if (intLightType == SPOT_LIGHT_TYPE)		// = 1
		{
			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
				= dot(vertexToLight.xyz, theLights[index].direction.xyz);

			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));

			// Is it completely outside of the spot?
			if (currentLightRayAngle < outerConeAngleCos)
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if (currentLightRayAngle < innerConeAngleCos)
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) /
					(innerConeAngleCos - outerConeAngleCos);

				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}

		}// if ( intLightType == 1 )



		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
			+ (vertexSpecular.rgb * lightSpecularContrib.rgb);

	}//for(intindex=0...

	finalObjectColour.a = 1.0f;

	return finalObjectColour;
}
