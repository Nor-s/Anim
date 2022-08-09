#ifndef GLCPP_OBB_COMPONENT_H
#define GLCPP_OBB_COMPONENT_H

#include "transform_component.h"
#include <glm/glm.hpp>

namespace glcpp
{
    class OBBComponent
    {
    public:
        // void get_box();

    private:
        glm::vec3 axis_len_;
        TransformComponent local_transform_;
        TransformComponent world_transform_;
    };

}

#endif