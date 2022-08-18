#ifndef ANIM_ENTITY_COMPONENT_MESH_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_MESH_COMPONENT_H

#include "../component.h"

#include <string>
#include <memory>
#include <vector>

namespace anim
{
    class Mesh;
    class Shader;
    class Entity;
    class MeshComponent : public ComponentBase<MeshComponent>
    {
    public:
        static inline bool isActivate = false;
        static inline bool isWireframe = false;
        ~MeshComponent() = default;
        void update() override;

        void set_meshes(const std::vector<std::shared_ptr<Mesh>> &meshes);
        void set_shader(std::shared_ptr<Shader> &shader);
        void set_entity(Entity *entity);

    private:
        std::vector<std::shared_ptr<Mesh>> meshes_;
        std::shared_ptr<Shader> shader_;
        Entity *entity_;
    };
}

#endif