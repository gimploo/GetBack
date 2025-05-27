#pragma once
#include "poglib/basic.h"
#include "poglib/util/glcamera.h"
#include "../game.h"

#define PLAYER_SPEED 4.0f

typedef struct {
    bool is_walking;
    vec3f_t delta_pos;
    matrix4f_t model_scale;
    vec3f_t pos;
    matrix4f_t model_transform;
} player_state_t;

typedef struct {
    glshader_t general_shader;
    glshader_t model_shader;
    glcamera_t camera;
    glmodel_t model;

    player_state_t state;
} main_scene_t;


matrix4f_t calculate_player_transformation(main_scene_t *game)
{
    vec3f_t move_dir = game->state.delta_pos;

    // Step 1: Only rotate if there's movement input
    if (glms_vec3_norm(move_dir) < 0.0001f) {
        // No movement, return translation only (no rotation)
        return glms_mat4_mul(glms_translate(MATRIX4F_IDENTITY, game->state.pos), game->state.model_scale);
    }

    // Step 2: Flatten movement on XZ plane (force Y = 0) and normalize
    move_dir.y = 0.0f;
    move_dir = glms_normalize(move_dir);

    // Step 3: Calculate rotation angle to face movement direction (assuming +Z forward)
    float angle = atan2f(move_dir.x, move_dir.z);  // Note: (x, z), not (z, x)

    // Step 4: Rotation around Y axis
    matrix4f_t rot = glms_rotate_y(MATRIX4F_IDENTITY, angle);

    // Step 5: Translation to player's position
    matrix4f_t trans = glms_translate(MATRIX4F_IDENTITY, game->state.pos);

    // Step 6: Combine translation and rotation
    matrix4f_t model = glms_mat4_mul(trans, glms_mat4_mul(rot, game->state.model_scale));

    return model;
}

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
                (vec3f_t ){ -125.f, 40.0f, 87.0f },
                (vec2f_t ){ -0.3f, -0.9f }), 
            .model = glmodel_init(model_file_path),
            .state = {
                .is_walking = false,
                .model_scale = glms_scale(MATRIX4F_IDENTITY, (vec3f_t){20.0f, 20.0f, 20.0f}),
                .delta_pos = vec3f(0.f),
                .pos = vec3f(0.f),
                .model_transform = MATRIX4F_IDENTITY
            }
        },
        sizeof(main_scene_t));
}

void main_scene_input(struct scene_t *s, const f32 dt)
{
    window_t *win = poggen_get_window(scene_get_engine());
    main_scene_t *content = s->content;

    //reset state
    content->state.is_walking = false;
    content->state.delta_pos = vec3f(0.f);

    if (window_keyboard_is_key_pressed(win, SDLK_w)) {
        content->state.is_walking = true;
        content->state.delta_pos.z -= PLAYER_SPEED * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_s)) {
        content->state.is_walking = true;
        content->state.delta_pos.z += PLAYER_SPEED * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_a)) {
        content->state.is_walking = true;
        content->state.delta_pos.x -= PLAYER_SPEED * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_d)) {
        content->state.is_walking = true;
        content->state.delta_pos.x += PLAYER_SPEED * dt;
    }

    content->state.pos = glms_vec3_add(content->state.pos, content->state.delta_pos);
    content->state.model_transform = calculate_player_transformation(content);

    glcamera_process_input(&content->camera, dt);
}

void main_scene_update(struct scene_t *s, const f32 dt) 
{ 
    main_scene_t *content = s->content;
    if (content->state.is_walking) {
        glmodel_set_animation(&content->model, "Walk_Formal_Loop", dt, true);
    } else {
        glmodel_set_animation(&content->model, "Idle_Loop", dt, true);
    }
}

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

    const matrix4f_t view = glcamera_getview(&game->camera);
    const matrix4f_t proj = glms_perspective(
        radians(45), 
        global_poggen->handle.app->window.aspect_ratio, 
        1.0f, 1000.0f
    );


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
                                .value = view
                            },
                            [1] = {
                                .name = "projection",
                                .type = "matrix4f_t",
                                .value = proj
                            },
                            [2] = {
                                .name = "transform",
                                .type = "matrix4f_t",
                                .value = game->state.model_transform
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
                                .value = view
                            },
                            [1] = {
                                .name = "projection",
                                .type = "matrix4f_t",
                                .value = proj
                            },
                            [2] = {
                                .name = "transform",
                                .type = "matrix4f_t",
                                .value = game->state.model_transform
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
