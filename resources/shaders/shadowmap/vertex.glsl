#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
//in vec4 new_vertex_color;
//in vec2 aTexCoords;
//in vec3 aNormal;
//in vec3 aPos;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // (Camera Position) 64
    mat4 view; // 128
    mat4 proj; // 144
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = proj * view * vec4(vs_out.FragPos, 1.0);
}