#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // (Camera Position) 64
    mat4 view; // 128
    mat4 proj; // 144
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;

    bool enabled;
};

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

const int NR_LIGHTS = 32;
uniform PointLight point_lights[32];
uniform Material material;

vec3 calculate_point_light(PointLight light, vec3 diffuse_texel, vec3 specular_texel, vec3 FragPos, vec3 Normal) {
    float light_distance = length(light.position - FragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * light_distance + light.quadratic * (light_distance * light_distance));
    float attenuation = light.intensity / max(light_distance * light_distance, 0.01);

    vec3 light_direction = normalize(light.position - FragPos);
    vec3 view_direction = normalize(c_pos - FragPos);

    vec3 normal = normalize(Normal);
    vec3 halfway = normalize(light_direction + view_direction);
    float specular = pow(max(dot(normal, halfway), 0.0), material.shininess);


    vec3 diffuse_light = (diffuse_texel * max(dot(normalize(Normal), light_direction), 0.0)) * light.diffuse;
    vec3 ambient_light = light.ambient * diffuse_texel ;
    vec3 specular_light = (specular_texel * pow(max(dot(normal, halfway), 0.0), material.shininess)) * light.specular;

    diffuse_light *= attenuation;
    ambient_light *= attenuation;
    specular_light *= attenuation;

    return (ambient_light+diffuse_light+specular_light);
}


void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(c_pos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
//        // calculate distance between light source and current fragment
//        float distance = length(lights[i].Position - FragPos);
//        if(distance < lights[i].Radius)
//        {
//            // diffuse
//            vec3 lightDir = normalize(lights[i].Position - FragPos);
//            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
//            // specular
//            vec3 halfwayDir = normalize(lightDir + viewDir);
//            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
//            vec3 specular = lights[i].Color * spec * Specular;
//            // attenuation
//            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
//            diffuse *= attenuation;
//            specular *= attenuation;
//            lighting += diffuse + specular;
//        }
        if (point_lights[i].enabled) {
            lighting += calculate_point_light(point_lights[i], Diffuse, vec3(Specular), FragPos, Normal);
        }
    }
    FragColor = vec4(lighting, 1.0);
}
