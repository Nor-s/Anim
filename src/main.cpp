#include "pixel3D.h"

int main()
{
    std::setlocale(LC_CTYPE, "");
    Pixel3D::get_instance()->init(1400, 800, "pixel3d");
    Pixel3D::get_instance()->loop();
    Pixel3D::destroy_instance();
    return 0;
}