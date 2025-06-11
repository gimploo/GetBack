#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 WorldPos;

struct light_t {
    vec3 position;
    float ambient;
    vec4 color;
};

struct material_t {
    vec4 color;
};

uniform light_t light;
uniform material_t material;

void main()
{
    vec3 norm = normalize(Normal);

    vec3 light_direction = normalize(light.position - WorldPos);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 result = (light.ambient + diff) * vec3(light.color);

    FragColor = vec4(result * material.color.xyz, 1.0);
}
