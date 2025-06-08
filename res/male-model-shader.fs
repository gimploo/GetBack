#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 light_position;
uniform vec4 light_color;
uniform vec4 diffuse_color;

void main()
{
    vec3 lightDirection = normalize(FragPos - light_position);
    vec4 diffuse = max(dot(lightDirection, Normal), 0.0) * light_color;
    FragColor = diffuse * diffuse_color;
}
