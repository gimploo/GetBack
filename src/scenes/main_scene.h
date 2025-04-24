#pragma once
#include "poglib/basic.h"
#include "../game.h"

typedef struct {
    glshader_t shader;
} main_scene_t;

void main_scene_init(struct scene_t * s)
{
    scene_pass_content(
        s, 
        &(main_scene_t ) {
            .shader = glshader_default_init(),
        }, 
        sizeof(main_scene_t)
    );
}

void main_scene_input(struct scene_t *s, const f32 dt)
{
}

void main_scene_update(struct scene_t *s, const f32 dt)
{
}

void main_scene_render(struct scene_t *s)
{
    const main_scene_t *game = (const main_scene_t *)s->content;

    glrenderer3d_draw_cube(&(glrenderer3d_t) {
        .shader = &game->shader,
    });
}

void main_scene_destroy(struct scene_t *s)
{
    main_scene_t *game = (const main_scene_t *)s->content;
    glshader_destroy(&game->shader);
}
