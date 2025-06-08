#pragma once
#include "../game.h"
#include "poglib/basic.h"
#include "poglib/util/glcamera.h"
#include <poglib/util/workbench.h>

#define PLAYER_SPEED 4.0f

typedef struct {
    bool is_walking;
    vec3f_t delta_pos;
    matrix4f_t scale;
    vec3f_t pos;
    matrix4f_t transform;
    glcamera_t camera;
} player_state_t;

typedef struct {
    workbench_t wb;
    glshader_t model_shader;
    glmodel_t model;

    bool is_debug_view;
    player_state_t player;

    gllight_t lightsource;

    str_t model_file_path;
} main_scene_t;

matrix4f_t calculate_player_transformation(main_scene_t *game) {
    vec3f_t move_dir = game->player.delta_pos;

    // Step 1: Only rotate if there's movement input
    if (glms_vec3_norm(move_dir) < 0.0001f) {
        // No movement, return translation only (no rotation)
        return glms_mat4_mul(glms_translate(MATRIX4F_IDENTITY, game->player.pos),
                             game->player.scale);
    }

    // Step 2: Flatten movement on XZ plane (force Y = 0) and normalize
    move_dir.y = 0.0f;
    move_dir = glms_normalize(move_dir);

    // Step 3: Calculate rotation angle to face movement direction (assuming +Z
    // forward)
    float angle = atan2f(move_dir.x, move_dir.z); // Note: (x, z), not (z, x)

    // Step 4: Rotation around Y axis
    matrix4f_t rot = glms_rotate_y(MATRIX4F_IDENTITY, angle);

    // Step 5: Translation to player's position
    matrix4f_t trans = glms_translate(MATRIX4F_IDENTITY, game->player.pos);

    // Step 6: Combine translation and rotation
    matrix4f_t model =
        glms_mat4_mul(trans, glms_mat4_mul(rot, game->player.scale));

    return model;
}

void main_scene_init(struct scene_t *s) {

    str_t model_file_path = application_get_absolute_filepath(global_poggen->handle.app, "res/male-model.glb");

    scene_pass_content(
        s,
        &(main_scene_t){
            .model_file_path = model_file_path,
            .lightsource = {
                .position = (vec3f_t){
                    .x = 0.0f,
                    .y = 5.0f,
                    .z = -2.f
                },
                .color = COLOR_CYAN,
            },
            .model_shader = glshader_from_cstr_init(SHADER3D_MODEL_VS, SHADER3D_MODEL_FS),
            .wb = workbench_init(global_poggen->handle.app),
            .is_debug_view = false,
            .model = glmodel_init(model_file_path.data),
            .player = {
                .is_walking = false,
                .scale = glms_scale(MATRIX4F_IDENTITY, (vec3f_t){20.0f, 20.0f, 20.0f}),
                .delta_pos = vec3f(0.f),
                .pos = vec3f(0.f),
                .transform = MATRIX4F_IDENTITY,
                .camera = glcamera_perspective(vec3f(0.f), vec2f(0.f))
            }
        }, 
        sizeof(main_scene_t)
    );


    main_scene_t *content = s->content;
    workbench_track_lightsource(&content->wb, &content->lightsource);
}

void main_scene_input(struct scene_t *s, const f32 dt) {
    window_t *win = poggen_get_window(scene_get_engine());
    main_scene_t *content = s->content;

    // reset state
    content->player.is_walking = false;
    content->player.delta_pos = vec3f(0.f);

    if (window_keyboard_is_key_just_pressed(win, SDLK_SLASH)) {
        content->is_debug_view = !content->is_debug_view;
    }

    if (content->is_debug_view && window_keyboard_is_key_just_pressed(win, SDLK_SPACE)) {
        workbench_toggle_wireframe_mode(&content->wb);
    }

    if (window_keyboard_is_key_pressed(win, SDLK_w)) {
        content->player.is_walking = true;
        content->player.delta_pos.z -= PLAYER_SPEED * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_s)) {
        content->player.is_walking = true;
        content->player.delta_pos.z += PLAYER_SPEED * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_a)) {
        content->player.is_walking = true;
        content->player.delta_pos.x -= PLAYER_SPEED * dt;
    }

    if (window_keyboard_is_key_pressed(win, SDLK_d)) {
        content->player.is_walking = true;
        content->player.delta_pos.x += PLAYER_SPEED * dt;
    }

    content->player.pos =
        glms_vec3_add(content->player.pos, content->player.delta_pos);
    content->player.transform = calculate_player_transformation(content);

    content->player.camera.position = (vec3f_t){
        content->player.pos.x,
        content->player.pos.y + 20,
        content->player.pos.z + 120,
    };
    content->player.camera.direction.front = GL_CAMERA_DIRECTION_FRONT;
    content->player.camera.direction.up = GL_CAMERA_DIRECTION_UP;

    if (content->is_debug_view) {
        glcamera_process_input(&content->wb.world_camera, dt);
    } else {
        glcamera_process_input(&content->player.camera, dt);
    }
}

void main_scene_update(struct scene_t *s, const f32 dt) {
    main_scene_t *content = s->content;
    if (content->is_debug_view) {
        workbench_pass_line(
            &content->wb,
            (line_t) {
                .start = content->player.camera.position,
                .end = glms_vec3_add(content->player.camera.position, glms_vec3_scale(content->player.camera.direction.front, 3.0f))
            }
        );
        workbench_update_player_camera_position(
            &content->wb, 
            content->player.camera.position);
    }
    if (content->player.is_walking) {
        glmodel_set_animation(&content->model, "Walk_Formal_Loop", dt, true);
    } else {
        glmodel_set_animation(&content->model, "Idle_Loop", dt, true);
    }
}

void main_scene_render(struct scene_t *s, const f32 dt) {
    main_scene_t *game = (main_scene_t *)s->content;

    const matrix4f_t view = glcamera_getview(&game->player.camera);
    const matrix4f_t proj = glms_perspective(
        radians(45), global_poggen->handle.app->window.aspect_ratio, 1.0f,
        1000.0f);

    glrenderer3d_draw_model(
        &game->model,
        (glshaderconfiglist_t){
            .count = 2,
            .configs = {
                [0] = (glshaderconfig_t){
                    .shader = &game->model_shader,
                    .uniforms = {
                        .count = 5,
                        .uniform = {
                            [0] = {
                                .name = "view", 
                                .type = "matrix4f_t", 
                                .value = game->is_debug_view ? glcamera_getview(&game->wb.world_camera) : view
                            },
                            [1] = {
                                .name = "projection",
                                .type = "matrix4f_t",
                                .value = proj
                            },
                            [2] = {
                                .name = "transform",
                                .type = "matrix4f_t",
                                .value = game->player.transform
                            },
                            [3] = {
                                .name = "diffuse_color",
                                .type = "vec4f_t",
                                .value.vec4 = *(vec4f_t *)list_get_value(&game->model .colors, 0)
                            },
                            [4] = {
                                .name = "uBones",
                                .type = "matrix4f_t []",
                                .value
                                .mat4s = {
                                    .count = game->model .transforms[0] .len, 
                                    .data = (matrix4f_t *)game ->model .transforms [0] .data
                                }
                            }
                        }
                    }
                },
                [1] = (glshaderconfig_t){
                    .shader = &game->model_shader,
                    .uniforms = {
                        .count = 5,
                        .uniform = {
                            [0] = {
                                .name = "view",
                                .type = "matrix4f_t",
                                .value = game->is_debug_view ? glcamera_getview(&game->wb.world_camera) : view
                            },
                            [1] = {
                                .name = "projection",
                                .type = "matrix4f_t",
                                .value = proj
                            },
                            [2] = {
                                .name = "transform",
                                .type = "matrix4f_t",
                                .value = game->player
                                .transform
                            },
                            [3] = {
                                .name = "diffuse_color",
                                .type = "vec4f_t",
                                .value.vec4 = *(vec4f_t *)list_get_value(&game->model .colors, 1)
                            },
                            [4] = {
                                .name = "uBones",
                                .type = "matrix4f_t []",
                                .value.mat4s = {
                                    .count = game->model .transforms[1] .len,
                                    .data = (matrix4f_t *) game->model .transforms[1] .data
                                }
                            }
                        }
                    }
                }
            }
        }
    );

    if (game->is_debug_view) {
        workbench_render(&game->wb);
    }
}

void main_scene_destroy(scene_t *s) {
    main_scene_t *game = (main_scene_t *)s->content;
    glmodel_destroy(&game->model);
    glshader_destroy(&game->model_shader);
    workbench_destroy(&game->wb);
    str_free(&game->model_file_path);
}
