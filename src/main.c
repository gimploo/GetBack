#define WINDOW_SDL
#include <poglib/application.h>

typedef struct content_t {

    const char *text;

} content_t ;

void GetBack_init(application_t *app) 
{
    content_t c = {
        .text = "Hello world\n" 
    };

    application_pass_content(app, &c);
}

void GetBack_update(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    window_update_user_input(win);
}

void GetBack_render(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);
}

void GetBack_destroy(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);
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

