#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "app.h"

int main()
{
    setlocale(LC_ALL, ".utf8");
    App::get_instance()->init(1400, 800, "Anim");
    App::get_instance()->loop();
    App::destroy_instance();
    return 0;
}