#version 330 core

layout (location = 0) in vec3 vertices;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangents;
layout (location = 4) in vec3 bitangents;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;
uniform mat4 uBones[53];

out vec3 Normal;
out vec3 WorldPos;

void main()
{
    vec4 pos = vec4(0.0);
    vec4 skinnedNormal = vec4(0.0);
    for(int i = 0; i < 4; i++) {
       if(boneIds[i] > 0) {
           pos += uBones[boneIds[i]] * vec4(vertices, 1.0) * boneWeights[i];
           skinnedNormal += uBones[boneIds[i]] * vec4(normal, 1.0) * boneWeights[i];
       }
    }
    vec4 worldPos = transform * pos;
    gl_Position = projection * view * worldPos;
    WorldPos = vec3(worldPos);
    Normal = vec3(skinnedNormal);
};

