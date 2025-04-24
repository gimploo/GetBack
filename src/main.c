#define WINDOW_SDL
#include <poglib/application.h>
#include "game.h"
#include "scenes/main_scene.h"

typedef struct content_t {

    GetBack game;

} content_t ;

void GetBack_init(application_t *app) 
{
    content_t c = {
        .game = {
            .engine = poggen_init(app)
        }
    };

    poggen_add_scene(c.game.engine, main_scene);

    application_pass_content(app, &c);
}

void GetBack_update(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    window_update_user_input(win);
    poggen_update(c->game.engine, application_get_dt(app));
}

void GetBack_render(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    poggen_render(c->game.engine);
}

void GetBack_destroy(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    poggen_destroy(c->game.engine);
}

int main(void)
{
    application_t app = {
        .window = {
            .title              = "GetBack",
            .width              = 1080,
            .height             = 920,
            .aspect_ratio       = 1080.f / 920.f,
            .fps_limit          = 60,
            .background_color   = COLOR_GREEN
        },   
        .content = {
            .size = sizeof(content_t )
        },
        .init       = GetBack_init,
        .update     = GetBack_update,
        .render     = GetBack_render,
        .destroy    = GetBack_destroy
    };

    application_run(&app);

    return 0;
}

