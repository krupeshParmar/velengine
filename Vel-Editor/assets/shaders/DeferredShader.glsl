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
in vec2 v_TextureCoords;

uniform float u_TilingFactor;
uniform sampler2D gAlbedo;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gSpecular;
uniform vec4 eyeLocation;
uniform vec2 screen_width_height;					// x = width, y = height

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
uniform Light theLights[NUMBEROFLIGHTS];


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
	vec3 FragPos = texture(gPosition, v_TextureCoords).rgb;
	vec3 Normal = texture(gNormal, v_TextureCoords).rgb;
	vec3 Diffuse = texture(gAlbedo, v_TextureCoords).rgb;
	vec4 Specular = texture(gSpecular, v_TextureCoords);

	vec4 outColour = calculateLightContrib(Diffuse, Normal,
		FragPos, Specular);

	color = vec4(outColour.rgb, 1.0);

	float ambientLight = 0.08f;
	color.rgb += (Diffuse * ambientLight);
	f_position = vec4(FragPos, 1.0);

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
			float diff = max(dot(norm, -lightDir), 0.0);
			vec3 diffuse = theLights[index].diffuse.xyz * diff;

			// specular
			/*vec3 viewDir = normalize(eyeLocation.xyz - vertexWorldPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0f);

			vec3 specular = theLights[index].specular.xyz * spec;*/

			finalObjectColour.rgb += (vertexMaterialColour.rgb * diffuse);
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
		float objectSpecularPower = vertexSpecular.r;

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



		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
			+ (vertexSpecular.rgb * lightSpecularContrib.rgb);

	}//for(intindex=0...

	finalObjectColour.a = 1.0f;

	return finalObjectColour;
}
