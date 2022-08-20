#include "mesh_component.h"
#include "../../../graphics/shader.h"
#include "../../../graphics/mesh.h"
#include "../../entity.h"
namespace anim
{
    void MeshComponent::update()
    {
        if (!isActivate)
        {
            return;
        }
        shader_->use();
        shader_->set_mat4("model", entity_->get_world_transformation());
        shader_->set_vec3("selectionColor", selectionColor);

        for (auto &mesh : meshes_)
        {
            if (entity_->is_selected_ || entity_->get_mutable_root()->is_selected_)
                mesh->draw_outline(*shader_);
            else
            {

                mesh->draw(*shader_);
            }
        }
    }
    void MeshComponent::set_meshes(const std::vector<std::shared_ptr<Mesh>> &meshes)
    {
        meshes_ = meshes;
    }
    void MeshComponent::set_shader(Shader *shader)
    {
        shader_ = shader;
    }
    void MeshComponent::set_entity(Entity *entity)
    {
        entity_ = entity;
    }
}