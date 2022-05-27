#include "pixel3D.h"

int main()
{
    setlocale(LC_ALL, ".utf8");
    Pixel3D::get_instance()->init(1400, 800, "pixel3d");
    Pixel3D::get_instance()->loop();
    Pixel3D::destroy_instance();
    return 0;
}