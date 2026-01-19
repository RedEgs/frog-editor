#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coordinate;

out vec2 new_texture_coordinate;
out vec3 new_vertex_normal;
out vec3 new_frag_position;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
   new_texture_coordinate = texture_coordinate;
   new_vertex_normal = vertex_normal;
   new_frag_position = vec3(model * vec4(vertex_position, 1.0));
   gl_Position = proj * view * model * vec4(vertex_position, 1.0);
}
