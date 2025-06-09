#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 WorldPos;

struct light_t {
    vec3 position;
    vec4 color;
};

struct material_t {
    vec4 color;
};

uniform light_t light;
uniform material_t material;

void main()
{
    //TODO: this needs to go
    const float ambient = 0.2;

    vec3 lightDirection = normalize(light.position - WorldPos);
    float diffuse = max(dot(normalize(Normal), lightDirection), 0.0);
    vec3 result = vec3((diffuse + ambient) * light.color * material.color);
    FragColor = vec4(result, 1.0);
}
