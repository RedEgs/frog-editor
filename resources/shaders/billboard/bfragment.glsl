#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec2 UvCoords;

uniform sampler2D tex;


layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // (Camera Position) 64
    mat4 view; // 128
    mat4 proj; // 144
};

void main()
{
    FragColor = texture(tex, UvCoords);
}