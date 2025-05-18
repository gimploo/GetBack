#pragma once
#include "poglib/basic.h"
#include "poglib/util/glcamera.h"
#include "../game.h"

typedef struct {
    glshader_t general_shader;
    glshader_t model_shader;
    glcamera_t camera;
    glmodel_t model;
    matrix4f_t model_transform;
} main_scene_t;

void main_scene_init(struct scene_t * s)
{
#ifdef _WIN64
    const char *model_file_path = "C:\\Users\\Gokul\\projects\\GetBack\\res\\male-model.glb";
#else
    const char *model_file_path = "/home/simploo/dev/projects/GetBack/res/male-model.glb";
#endif

    scene_pass_content(
        s,
        &(main_scene_t ) {
            .general_shader = glshader_from_cstr_init(SHADER3D_VS, SHADER3D_FS),
            .model_shader = glshader_from_cstr_init(SHADER3D_MODEL_VS, SHADER3D_MODEL_FS),
            .camera = glcamera_perspective(
                (vec3f_t ){ 8.0f, 16.0f, 36.0f },
                (vec2f_t ){ radians(0), radians(-40) }), 
            .model_transform = glms_scale(MATRIX4F_IDENTITY, (vec3f_t){20.0f, 20.0f, 20.0f}),
            .model = glmodel_init(model_file_path)
        },
        sizeof(main_scene_t));
}

void main_scene_input(struct scene_t *s, const f32 dt)
{
    window_t *win = poggen_get_window(scene_get_engine());
    main_scene_t *content = s->content;

    glcamera_process_input(&content->camera, dt);
}

void main_scene_update(struct scene_t *s, const f32 dt) { }

glrendercall_t get_platform_render_config(main_scene_t *game)
{
    return (glrendercall_t ){
        .shader_config = {
            .shader = &game->general_shader,
            .uniforms = {
                .count = 4,
                .uniform = {
                    [0] = {
                        .name = "view",
                        .type = "matrix4f_t",
                        .value = glcamera_getview(&game->camera)
                    },
                    [1] = {
                        .name = "projection",
                        .type = "matrix4f_t",
                        .value = glms_perspective(
                            radians(45), 
                            global_poggen->handle.app->window.aspect_ratio, 
                            1.0f, 1000.0f
                        )
                    },
                    [2] = {
                        .name = "transform",
                        .type = "matrix4f_t",
                        .value = glms_scale(MATRIX4F_IDENTITY, (vec3f_t){1000.0f, 1.0f, 1000.0f}),
                    },
                    [3] = {
                        .name = "color",
                        .type = "vec4f_t",
                        .value.vec4 = COLOR_DARK_GRAY
                    }
                }
            }
        },
        .vtx = {
            .data = (u8 *)CUBE_VERTICES,
            .size = sizeof(CUBE_VERTICES)
        },
        .idx = {
            .data = (u8 *)CUBE_INDICES,
            .nmemb = ARRAY_LEN(CUBE_INDICES)
        },
        .attrs = {
            .count = 1,
            .attr = {
                [0] = {
                    .ncmp = 3,
                    .interleaved = {0} 
                }
            },

        },
        .textures = {0},
    };
}

void main_scene_render(struct scene_t *s, const f32 dt)
{
    main_scene_t *game = (main_scene_t *)s->content;

    glrenderer3d_draw((glrendererconfig_t) {
        .calls = {
            .count = 1,
            .call = {
                [0] = get_platform_render_config(game)
            }
        }
    });

    glrenderer3d_draw_model(
        &game->model,
        (glshaderconfiglist_t) {
            .count = 2,
            .configs = {
                [0] = (glshaderconfig_t ){
                    .shader = &game->model_shader,
                    .uniforms = {
                        .count = 5,
                        .uniform = {
                            [0] = {
                                .name = "view",
                                .type = "matrix4f_t",
                                .value = glcamera_getview(&game->camera)
                            },
                            [1] = {
                                .name = "projection",
                                .type = "matrix4f_t",
                                .value = glms_perspective(
                                    radians(45), 
                                    global_poggen->handle.app->window.aspect_ratio, 
                                    1.0f, 1000.0f
                                )
                            },
                            [2] = {
                                .name = "transform",
                                .type = "matrix4f_t",
                                .value = game->model_transform
                            },
                            [3] = {
                                .name = "diffuse_color",
                                .type = "vec4f_t",
                                .value.vec4 = *(vec4f_t *)list_get_value(&game->model.colors, 0)
                            },
                            [4] = {
                                .name = "uBones",
                                .type = "matrix4f_t []",
                                .value.mat4s = {
                                    .count = game->model.transforms[0].len,
                                    .data = (matrix4f_t *)game->model.transforms[0].data
                                }
                            }
                        }
                    }
                },
                [1] = (glshaderconfig_t ) {
                    .shader = &game->model_shader,
                    .uniforms = {
                        .count = 5,
                        .uniform = {
                            [0] = {
                                .name = "view",
                                .type = "matrix4f_t",
                                .value = glcamera_getview(&game->camera)
                            },
                            [1] = {
                                .name = "projection",
                                .type = "matrix4f_t",
                                .value = glms_perspective(
                                    radians(45), 
                                    global_poggen->handle.app->window.aspect_ratio, 
                                    1.0f, 1000.0f
                                )
                            },
                            [2] = {
                                .name = "transform",
                                .type = "matrix4f_t",
                                .value = game->model_transform
                            },
                            [3] = {
                                .name = "diffuse_color",
                                .type = "vec4f_t",
                                .value.vec4 = *(vec4f_t *)list_get_value(&game->model.colors, 1)
                            },
                            [4] = {
                                .name = "uBones",
                                .type = "matrix4f_t []",
                                .value.mat4s = {
                                    .count = game->model.transforms[1].len,
                                    .data = (matrix4f_t *)game->model.transforms[1].data
                                }
                            }
                        }
                    }
                }

            }
        }
    );

}

void main_scene_destroy(scene_t *s)
{
    main_scene_t *game = (main_scene_t *)s->content;
    glmodel_destroy(&game->model);
    glshader_destroy(&game->model_shader);
    glshader_destroy(&game->general_shader);
}
