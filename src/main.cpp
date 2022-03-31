#include "pixel3D.h"

int main()
{
    Pixel3D::get_instance()->init(1400, 800, "pixel3d");
    Pixel3D::get_instance()->loop();
    return 0;
}