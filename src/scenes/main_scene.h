#pragma once
#include "poglib/basic.h"
#include "poglib/util/glcamera.h"
#include "../game.h"

typedef struct {
    glshader_t shader;
    glcamera_t camera;
    vec3f_t platormPosition;
} main_scene_t;

void main_scene_init(struct scene_t * s)
{
    scene_pass_content(
        s,
        &(main_scene_t ) {
            .shader = glshader_from_cstr_init(SHADER3D_VS, SHADER3D_FS),
            .camera = glcamera_perspective(
                (vec3f_t ){8.0f, 16.0f, 36.0f},
                (vec2f_t ){radians(0), radians(-40)}), 
        },
        sizeof(main_scene_t));
}

void main_scene_input(struct scene_t *s, const f32 dt)
{
    window_t *win = poggen_get_window(scene_get_engine());
    main_scene_t *content = s->content;

    if (window_keyboard_is_key_just_pressed(win, SDLK_UP))
    {
        content->platormPosition.z -= 0.5f * dt; 
    }

    if (window_keyboard_is_key_just_pressed(win, SDLK_DOWN))
    {
        content->platormPosition.z += 0.5f * dt; 
    }
}

void main_scene_update(struct scene_t *s, const f32 dt)
{
}

void main_scene_render(struct scene_t *s)
{
    const main_scene_t *game = (const main_scene_t *)s->content;

    glrenderer3d_draw((glrendererconfig_t) {
        .calls = {
            .count = 1,
            .call = {
                [0] = {
                    .shader = &game->shader,
                    .vtx = {
                        .data = CUBE_VERTICES,
                        .size = sizeof(CUBE_VERTICES)
                    },
                    .idx = {
                        .data = CUBE_INDICES,
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
                    .uniforms = {
                        .count = 3,
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
                                .value = matrix4f_translation(game->platormPosition)
                            },
                        }
                    }
                }
            }
        }
    });
}

void main_scene_destroy(struct scene_t *s)
{
    main_scene_t *game = (const main_scene_t *)s->content;
    glshader_destroy(&game->shader);
}
