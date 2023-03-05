#type vertex
#version 420 core
layout(location = 0)in vec4 vColour;
layout(location = 1)in vec4 vPosition;
layout(location = 2)in vec4 vNormal;
layout(location = 3)in vec4 vUVx2;
layout(location = 4)in vec4 vTangent;
layout(location = 5)in vec4 vBiNormal;
layout(location = 6)in vec4 vBoneID;
layout(location = 7)in vec4 vBoneWeight;


out vec3 TexCoords;

uniform mat4 viewprojection;

void main()
{
    TexCoords = vPosition.xyz;
    vec4 pos = viewprojection * vec4(vPosition.xyz, 1.0);
    gl_Position = pos.xyww;
}

#type fragment
#version 420 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 tilingfactor;

void main()
{
    FragColor = texture(skybox, TexCoords);
}