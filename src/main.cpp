#include "app.h"

int main()
{
    setlocale(LC_ALL, ".utf8");
    App::get_instance()->init(1400, 800, "pixel3d");
    App::get_instance()->loop();
    App::destroy_instance();
    return 0;
}