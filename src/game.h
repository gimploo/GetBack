#pragma once
#include "poglib/poggen.h"

typedef struct GetBack {

    poggen_t * engine;

} GetBack;


const char * SHADER3D_VS = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertices;\n"
    "layout (location = 1) in vec3 normal;\n"
    "layout (location = 2) in vec2 uv;\n"

    "out vec2 frag_uv;"

    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 transform;\n"

    "void main()\n"
    "{\n"
    "   frag_uv = uv;"
    "   gl_Position = projection * view * transform * vec4(vertices, 1.0);\n"
    "}\n";

const char *SHADER3D_FS = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"

    "uniform vec4 color;"

    "void main()\n"
    "{\n"
    "       FragColor = color;\n"
    "}\n";

const char *SHADER3D_MODEL_FS = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"

    "in vec2 frag_uv;"

    "uniform vec4 diffuse_color;"

    "void main()\n"
    "{\n"
    "       FragColor = diffuse_color;\n"
    "}\n";


const f32 CUBE_VERTICES[] = {
    // Front face
    -1.0f, -1.0f, -1.0f,  // 0
    1.0f, -1.0f, -1.0f,   // 1
    1.0f,  1.0f, -1.0f,   // 2
    -1.0f,  1.0f, -1.0f,  // 3

    // Back face
    -1.0f, -1.0f,  1.0f,  // 4
    1.0f, -1.0f,  1.0f,   // 5
    1.0f,  1.0f,  1.0f,   // 6
    -1.0f,  1.0f,  1.0f,  // 7
};

const u32 CUBE_INDICES[] = {
    // Front face
    0, 1, 2, 2, 3, 0,

    // Back face
    4, 5, 6, 6, 7, 4,

    // Left face
    4, 7, 3, 3, 0, 4,

    // Right face
    1, 2, 6, 6, 5, 1,

    // Bottom face
    4, 5, 1, 1, 0, 4,

    // Top face
    3, 2, 6, 6, 7, 3
};

