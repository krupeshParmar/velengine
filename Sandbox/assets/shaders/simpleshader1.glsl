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

out vec2 v_TextureCoords; 
out vec4 v_Color;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    v_TextureCoords = vUVx2.xy;
    v_Color = vColour;
    gl_Position = u_ViewProjection * u_Transform * vec4(vPosition.xyz, 1.0);
}

#type fragment
#version 330 core
layout(location = 0) out vec4 color;
in vec2 v_TextureCoords;
in vec4 v_Color;
uniform sampler2D u_Texture;
uniform bool isAWall;
uniform bool isInControl;
uniform bool isGround;

float near = 0.1f;
float far = 100.f;


void main()
{

    if (isAWall)
    {
        color = vec4(texture(u_Texture, v_TextureCoords).xyz, 0.5);
    }
    else color = texture(u_Texture, v_TextureCoords);
}
