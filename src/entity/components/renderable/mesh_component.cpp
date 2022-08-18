#include "mesh_component.h"
#include "../../../graphics/shader.h"
#include "../../../graphics/mesh.h"
#include "../../entity.h"
namespace anim
{
    void MeshComponent::update()
    {
        if (isActivate)
        {
            return;
        }
        shader_->use();
        shader_->set_mat4("model", entity_->get_world_transformation());
        // apply transformation
        for (auto &mesh : meshes_)
        {
            mesh->draw(*shader_);
        }
    }
    void MeshComponent::set_meshes(const std::vector<std::shared_ptr<Mesh>> &meshes)
    {
        meshes_ = meshes;
    }
    void MeshComponent::set_shader(std::shared_ptr<Shader> &shader)
    {
        shader_ = shader;
    }
    void MeshComponent::set_entity(Entity *entity)
    {
        entity_ = entity;
    }
}