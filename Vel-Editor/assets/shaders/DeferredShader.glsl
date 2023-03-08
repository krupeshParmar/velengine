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

uniform float u_TilingFactor;
uniform sampler2D gAlbedo;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gSpecular;
uniform sampler2D gBloom;
uniform vec4 eyeLocation;
uniform vec2 screen_width_height;					// x = width, y = height
uniform bool UseBloom;

struct Light
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 atten;     // x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = On or Off
	float intensity;
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 20;
uniform Light theLights[NUMBEROFLIGHTS];
uniform float Exposure;
uniform float BloomIntensity;
uniform float BloomThreshold;
uniform float weights1[5] = float[](0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

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



vec4 calculateLightContrib(vec3 vertexMaterialColour, vec3 vertexNormal,
	vec3 vertexWorldPos, vec4 vertexSpecular);

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

void main()
{
	const float gamma = 2.2;
	vec3 FragPos = texture(gPosition, v_TextureCoords).rgb;
	vec4 Normal = texture(gNormal, v_TextureCoords);
	vec3 Diffuse = texture(gAlbedo, v_TextureCoords).rgb;
	vec4 Specular = texture(gSpecular, v_TextureCoords);

	vec4 outColour = calculateLightContrib(Diffuse, Normal.xyz,
		FragPos, Specular);

	color = vec4(outColour.rgb, 1.0);

	float ambientLight = 0.08f;
	color.rgb += (Diffuse * ambientLight);
	f_bloom = vec4(0.0, 0.0, 0.0, 1.0);

	if (dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f)) > BloomThreshold)
	{
		f_bloom.rgb = color.rgb;
	}

	vec3 result = vec3(0.0f, 0.f, 0.f);
	//if (UseBloom)
	//{
	//	vec2 tex_offset = 1.0f / textureSize(gBloom, 0);
	//	if (texture(gBloom, v_TextureCoords).w > 0.5f)
	//	{
	//		for (int k = 0; k < 5; k++)
	//		{
	//			for (int i = 0; i < N; ++i)
	//			{
	//				//result += texture(gBloom, v_TextureCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights2[i];
	//				//result += texture(gBloom, v_TextureCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights2[i];
	//				vec2 tc = v_TextureCoords + tex_offset * vec2(float(i - M), 0);
	//				result += weights2[i] * texture(gBloom, tc).rgb;
	//			}
	//
	//			for (int j = 0; j < N; ++j)
	//			{
	//				//result += texture(gBloom, v_TextureCoords + vec2(0.f, tex_offset.y * j)).rgb * weights2[j];
	//				//result += texture(gBloom, v_TextureCoords - vec2(0.f, tex_offset.y * j)).rgb * weights2[j];
	//				vec2 tc = v_TextureCoords + tex_offset * vec2(0, float(j - M));
	//				result += weights2[j] * texture(gBloom, tc).rgb;
	//			}
	//		}
	//
	//		//f_bloom = vec4(result, 1.0f);
	//	}
	//}

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
	vec3 toneMapped = vec3(1.0f) - exp(-color.rgb * Exposure);

	color.rgb = pow(toneMapped, vec3(1.0 / gamma));
	color.w = 1.0f;
	f_normal = vec4(Normal.xyz, 1.0);
	f_specular = Specular;

	/*float average = (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b) / 3.0;
	color = vec4(average, average, average, 1.0);*/
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
			//// This is supposed to simulate sunlight. 
			//// SO: 
			//// -- There's ONLY direction, no position
			//// -- Almost always, there's only 1 of these in a scene
			//// Cheapest light to calculate. 
			//
			//vec3 lightContrib = theLights[index].diffuse.rgb;
			//
			//// Get the dot product of the light and normalize
			//float dotProduct = dot(-theLights[index].direction.xyz,
			//	normalize(norm.xyz));	// -1 to 1
			//
			//dotProduct = max(0.0f, dotProduct);		// 0 to 1
			//
			//lightContrib *= dotProduct;
			//
			//finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib);
			//continue;

			// diffuse 
			// 
			// vec3 lightDir = normalize(light.position - FragPos);
			vec3 lightDir = -theLights[index].direction.xyz;
			lightDir = normalize(lightDir);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = theLights[index].diffuse.xyz * diff;

			// specular
			/*vec3 viewDir = normalize(eyeLocation.xyz - vertexWorldPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0f);

			vec3 specular = theLights[index].specular.xyz * spec;*/

			finalObjectColour.rgb += (vertexMaterialColour.rgb * diffuse * theLights[index].intensity);
				//+ (vertexSpecular.rgb * specular);

			continue;
		}

		// Assume it's a point light 
		// intLightType = 0

		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, norm.xyz);

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
		vec3 halfwayDir = normalize(lightVector + eyeVector.xyz);
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
				= dot(vertexToLight.xyz, normalize(theLights[index].direction.xyz));

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



		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb * theLights[index].intensity)
			+ (vertexSpecular.rgb * lightSpecularContrib.rgb);

	}//for(intindex=0...

	finalObjectColour.a = 1.0f;

	return finalObjectColour;
}
