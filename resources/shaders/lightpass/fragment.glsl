#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

const int NR_LIGHTS = 32;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gShadowmap[NR_LIGHTS];
uniform mat4 lightSpaceMatrices[NR_LIGHTS];

layout (std140) uniform Globals
{
    float time; // 0
    vec3 c_pos; // 64
    mat4 view;  // 128
    mat4 proj;  // 144
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
    float range;
    int cast_shadow;
    int enabled;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float intensity;
    int cast_shadow;
    int enabled;
};

uniform PointLight point_lights[NR_LIGHTS];
uniform DirectionalLight directional_lights[NR_LIGHTS];

// -------------------------
// Lighting calculations
// -------------------------
vec3 calculate_point_light(PointLight light, vec3 diffuse_texel, vec3 specular_texel, vec3 FragPos, vec3 Normal)
{
    vec3 light_dir = light.position - FragPos;
    float light_distance = length(light_dir);

    if (light_distance > light.range)
    return vec3(0.0);

    light_dir = normalize(light_dir);

    // Attenuation
    float attenuation = light.intensity / (light_distance * light_distance + 1.0);
    float edge = 1.0 - pow(light_distance / light.range, 4.0);
    attenuation *= edge;

    // View and halfway vectors
    vec3 viewDir = normalize(c_pos - FragPos);
    vec3 halfway = normalize(light_dir + viewDir);

    // Normals
    vec3 normal = normalize(Normal);

    // Diffuse and specular
    float diff = max(dot(normal, light_dir), 0.0);
    float spec = pow(max(dot(normal, halfway), 0.0), 16.0);

    vec3 ambient = diffuse_texel * light.ambient * attenuation;
    vec3 diffuse = diffuse_texel * light.diffuse * diff * attenuation;
    vec3 specular = specular_texel * light.specular * spec * attenuation;

    return ambient + diffuse + specular;
}

vec3 calculate_directional_light(DirectionalLight light, vec3 diffuse_texel, vec3 specular_texel, vec3 FragPos, vec3 Normal)
{
    vec3 light_dir = normalize(-light.direction);
    vec3 viewDir = normalize(c_pos - FragPos);
    vec3 halfway = normalize(light_dir + viewDir);
    vec3 normal = normalize(Normal);

    float diff = max(dot(normal, light_dir), 0.0);
    float spec = pow(max(dot(normal, halfway), 0.0), 16.0);

    vec3 ambient = diffuse_texel * light.ambient;
    vec3 diffuse = diffuse_texel * light.diffuse * diff;
    vec3 specular = specular_texel * light.specular * spec;

    return light.intensity * (ambient + diffuse + specular);
}

// -------------------------
// Shadow calculation
// -------------------------
float ShadowCalculation(int index, vec3 fragPos, vec3 normal)
{
    vec4 fragPosLightSpace = lightSpaceMatrices[index] * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
    return 0.0;

    float closestDepth = texture(gShadowmap[index], projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, normalize(vec3(lightSpaceMatrices[index][3]) - fragPos))), 0.005);

    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

// -------------------------
// Main
// -------------------------
void main()
{
    // Retrieve G-buffer data
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting = vec3(0.0);

    // -------------------------
    // Point lights
    // -------------------------
    for (int i = 0; i < NR_LIGHTS; ++i)
    {
        if (point_lights[i].enabled != 0)
        {
            lighting += calculate_point_light(
            point_lights[i],
            Diffuse,
            vec3(Specular),
            FragPos,
            Normal
            );
        }
    }

    // -------------------------
    // Directional lights
    // -------------------------
    for (int i = 0; i < NR_LIGHTS; ++i)
    {
        if (directional_lights[i].enabled == 0)
        continue;

        vec3 lightContribution = calculate_directional_light(
        directional_lights[i],
        Diffuse,
        vec3(Specular),
        FragPos,
        Normal
        );

        float shadow = 0.0;
        if (directional_lights[i].cast_shadow != 0)
        shadow = ShadowCalculation(i, FragPos, Normal);

        lighting += (1.0 - shadow) * lightContribution;
    }

    FragColor = vec4(lighting, 1.0);
}
