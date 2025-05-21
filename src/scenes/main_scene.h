#pragma once
#include "poglib/basic.h"
#include "poglib/util/glcamera.h"
#include "../game.h"

#define PLAYER_SPEED 0.2f

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

matrix4f_t calculate_player_transformation(main_scene_t *game, const matrix4f_t view, const matrix4f_t proj, const vec2f_t ndc, const f32 dt)
{
    const matrix4f_t inv = glms_mat4_inv(glms_mat4_mul(proj, view));

    // starts at the camera pos
    const vec3f_t ray_direction = glms_mat4_mulv3(inv, (vec3f_t ){ndc.x, ndc.y, -1.f}, 0.f);
    const vec3f_t ray_origin = game->camera.position;

    const vec3f_t plane_normal = { 0.f, 1.f, 0.f };
    const vec3f_t plane_point = { 0.f, 0.f, 0.f };

    const f32 intersection = glms_vec3_dot(
        glms_vec3_sub(plane_point, ray_origin), 
        plane_normal
    ) / glms_vec3_dot(ray_direction, plane_normal);

    // world space intersection point
    const vec3f_t intersection_point = glms_vec3_add(
        ray_origin, 
        glms_vec3_scale(ray_direction, intersection)
    );

    const vec3f_t direction = glms_vec3_sub(
        intersection_point, game->state.pos
    );

    const f32 angle = -1.f * atan2f(direction.z, direction.x);

    const vec3f_t norm_direction = glms_vec3_normalize(direction);
    game->state.pos = glms_vec3_add(glms_vec3_add(
        game->state.pos, 
        glms_vec3_scale(norm_direction, dt * PLAYER_SPEED)
    ), game->state.delta_pos);

    return glms_mat4_mul(
        glms_translate_make(
            game->state.pos
        ),
        glms_mat4_mul(
            glms_rotate_make(angle, plane_normal),
            game->state.model_scale
        )
    );
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

    const f32 movement_speed = 0.3f;
    
    //reset state
    content->state.is_walking = false;
    content->state.delta_pos = vec3f(0.f);

    if (window_keyboard_is_key_pressed(win, SDLK_w)) {
        content->state.is_walking = true;
        content->state.delta_pos.z += movement_speed * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_s)) {
        content->state.is_walking = true;
        content->state.delta_pos.z -= movement_speed * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_a)) {
        content->state.is_walking = true;
        content->state.delta_pos.x -= movement_speed * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_d)) {
        content->state.is_walking = true;
        content->state.delta_pos.x += movement_speed * dt;
    }

    const matrix4f_t view = glcamera_getview(&content->camera);
    const matrix4f_t proj = glms_perspective(
        radians(45), 
        global_poggen->handle.app->window.aspect_ratio, 
        1.0f, 1000.0f
    );

    content->state.model_transform = calculate_player_transformation(
        content, 
        view, 
        proj, 
        window_mouse_get_norm_position(win),
        dt
    );

    //glcamera_process_input(&content->camera, dt);
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
