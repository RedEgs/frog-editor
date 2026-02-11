
#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // (Camera Position) 64
    mat4 view; // 128
    mat4 proj; // 144
};

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

void main()
{
    gPosition = FragPos;                         // world-space position
    gNormal   = normalize(Normal);               // world-space normal
    gAlbedoSpec.rgb = texture(material.diffuse, TexCoords).rgb;  // diffuse color
    gAlbedoSpec.a   = texture(material.specular, TexCoords).r;   // specular intensity
}
