#include "shared_resources.h"

#include "../animation/animator.h"
#include "../animation/animation.h"
#include "../graphics/shader.h"
#include "../entity/entity.h"

namespace anim
{
    SharedResources::SharedResources() = default;
    SharedResources::~SharedResources() = default;
    Animator *SharedResources::get_mutable_animator()
    {
        return nullptr;
    }
    std::shared_ptr<Shader> SharedResources::get_mutable_shader(const std::string &name)
    {
        return nullptr;
    }
    void SharedResources::init_animator()
    {
    }

}