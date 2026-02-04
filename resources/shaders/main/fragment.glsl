#version 330 core

struct Material {
   vec3 ambient;
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

struct DirectionalLight {
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   bool enabled;
};

struct PointLight {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float intensity;

   bool enabled;
};

in vec4 new_vertex_color;
in vec2 new_texture_coordinate;
in vec3 new_vertex_normal;
in vec3 new_frag_position;

in vec3 texture_dir;

out vec4 fragment_color;

layout (std140) uniform Globals
{
   float time; // 0
   vec3 c_pos; // (Camera Position) 64
   mat4 view; // 128
   mat4 proj; // 144
};

uniform Material material;

uniform PointLight point_lights[16];
uniform DirectionalLight directional_lights[8];

float gamma = 2.2;

vec3 calculate_directional_light(DirectionalLight light, vec3 diffuse_texel, vec3 specular_texel) {
   vec3 view_direction = normalize(c_pos - new_frag_position);
   vec3 light_direction = normalize(-light.direction);

   vec3 normal = normalize(new_vertex_normal);
   float diffuse = max(dot(normal, light_direction), 0.0);

   vec3 halfway = normalize(light_direction + view_direction);
   float specular = pow(max(dot(normal, halfway), 0.0), material.shininess);

   vec3 ambient_light = light.ambient * diffuse_texel;
   vec3 diffuse_light = light.diffuse * diffuse * diffuse_texel;
   vec3 specular_light = light.specular * specular * specular_texel;

   return(ambient_light+diffuse_light+specular_light);

}

vec3 calculate_point_light(PointLight light, vec3 diffuse_texel, vec3 specular_texel) {
   float light_distance = length(light.position - new_frag_position);
   //float attenuation = 1.0 / (light.constant + light.linear * light_distance + light.quadratic * (light_distance * light_distance));
   float attenuation = light.intensity / max(light_distance * light_distance, 0.01);

   vec3 light_direction = normalize(light.position - new_frag_position);
   vec3 view_direction = normalize(c_pos - new_frag_position);

   vec3 normal = normalize(new_vertex_normal);
   vec3 halfway = normalize(light_direction + view_direction);
   float specular = pow(max(dot(normal, halfway), 0.0), material.shininess);


   vec3 diffuse_light = (diffuse_texel * max(dot(normalize(new_vertex_normal), light_direction), 0.0)) * light.diffuse;
   vec3 ambient_light = light.ambient * diffuse_texel ;
   vec3 specular_light = (specular_texel * pow(max(dot(normal, halfway), 0.0), material.shininess)) * light.specular;

   diffuse_light *= attenuation;
   ambient_light *= attenuation;
   specular_light *= attenuation;

   return (ambient_light+diffuse_light+specular_light);
}

float ProcessDepth(float depth) {
   float near = 0.1;
   float far  = 10.0;

   float z = depth * 2.0 - 1.0; // back to NDC
   return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
   vec3 result = vec3(0.0f);
   vec3 diffuse_texel = max(vec3(texture(material.diffuse, new_texture_coordinate)), vec3(0.1));
   vec3 specular_texel = max(vec3(texture(material.specular, new_texture_coordinate)), vec3(0.1));


   for (int i = 0; i < directional_lights.length(); i++) {
      if (directional_lights[i].enabled) {
         result += calculate_directional_light(directional_lights[i], diffuse_texel, specular_texel);
      }
   }

   for (int i = 0; i < point_lights.length(); i++) {
      if (point_lights[i].enabled) {
         result += calculate_point_light(point_lights[i], diffuse_texel, specular_texel);
      }
   }

   float depth = ProcessDepth(gl_FragCoord.z) / 1.0;

   fragment_color = vec4(pow(result.rgb, vec3(1.0/gamma)), 1.0);//* vec4(new_vertex_color.x, new_vertex_color.y, new_vertex_color.z, 1.0f);
}