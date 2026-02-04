#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 UV;

out vec3 TexCoords;
out vec2 UvCoords;

layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // (Camera Position) 64
    mat4 view; // 128
    mat4 proj; // 144
};

uniform mat4 model; // center of billboard;
uniform bool ylock;
uniform float size;

void main()
{
    UvCoords = UV;

    vec3 centerWS = model[3].xyz;

    vec3 right;
    vec3 up;

    if (ylock)
    {
        // Cylindrical (Y-locked)
        vec3 toCam = c_pos - centerWS;
        toCam.y = 0.0;
        toCam = normalize(toCam);

        up = vec3(0.0, 1.0, 0.0);
        right = normalize(cross(up, toCam));
    }
    else
    {
        // Spherical (full facing)
        mat4 invView = inverse(view);
        right = normalize(invView[0].xyz);
        up    = normalize(invView[1].xyz);
    }

    vec3 offset =
    right * pos.x * size +
    up    * pos.y * size;

    gl_Position = proj * view * vec4(centerWS + offset, 1.0);
}