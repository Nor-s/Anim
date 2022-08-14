#ifndef ANIM_ENTITY_COMPONENT_MESH_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_MESH_COMPONENT_H

#include "../component.h"
#include "graphics/mesh.h"

#include <string>

namespace anim
{
    class MeshComponent : public ComponentBase<MeshComponent>
    {
    public:
        MeshComponent() = default;
        virtual ~MeshComponent() = default;
        virtual void update() override
        {
            mesh_->draw(*shader_);
        }

    private:
        std::shared_ptr<Mesh> mesh_;
        std::shared_ptr<Shader> shader_;
    };
}

#endif