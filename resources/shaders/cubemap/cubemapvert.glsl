#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // (Camera Position) 64
    mat4 view; // 128
    mat4 proj; // 144
};

uniform mat4 cview;

void main()
{
    TexCoords = aPos;

    vec4 pos = proj * cview * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 