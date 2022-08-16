#include "shared_resources.h"

#include "../animation/animator.h"
#include "../animation/animation.h"
#include "../graphics/shader.h"
#include "../graphics/opengl/gl_shader.h"
#include "../entity/entity.h"
#include "model.h"
#include "importer.h"
#include "../entity/components/component.h"
#include "../entity/components/pose_component.h"
#include "../entity/components/renderable/mesh_component.h"
#include "../entity/components/renderable/armature_component.h"
#include "../entity/components/animation_component.h"
#include "../entity/components/transform_component.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace anim
{
    SharedResources::SharedResources()
    {
        init_animator();
        init_shader();
        root_entity_.reset(new Entity());
        root_entity_->set_name("All");
    }
    SharedResources::~SharedResources()
    {
        glDeleteBuffers(1, &matrices_UBO_);
    }
    Animator *SharedResources::get_mutable_animator()
    {
        return nullptr;
    }
    std::shared_ptr<Shader> SharedResources::get_mutable_shader(const std::string &name)
    {
        return shaders_[name];
    }
    void SharedResources::import(const char *path)
    {
        Importer import{};
        auto [model, animations] = import.read_file(path);

        add_animations(animations);
        add_entity(model);
    }
    void SharedResources::add_entity(std::shared_ptr<Model> &model)
    {
        if (!model)
        {
            return;
        }
        std::shared_ptr<Entity> entity = nullptr;
        convert_to_entity(entity, model, model->get_root_node(), nullptr);
        if (entity)
        {
            root_entity_->add_children(std::move(entity));
        }
    }
    void SharedResources::add_animations(const std::vector<std::shared_ptr<Animation>> &animations)
    {
        animations_.insert(animations_.end(), std::make_move_iterator(animations.begin()), std::make_move_iterator(animations.end()));
    }
    void SharedResources::add_shader(const std::string &name, const char *vs_path, const char *fs_path)
    {
        shaders_[name] = std::make_shared<gl::GLShader>(vs_path, fs_path);
    }
    void SharedResources::update()
    {
        root_entity_->update();
    }
}

namespace anim
{
    void SharedResources::init_animator()
    {
        animator_ = std::make_unique<Animator>();
    }
    void SharedResources::init_shader()
    {
        add_shader("model", "./resources/shaders/1.model_loading.vs", "./resources/shaders/1.model_loading.fs");
        add_shader("animation", "./resources/shaders/animation_loading.vs", "./resources/shaders/1.model_loading.fs");
        add_shader("framebuffer", "./resources/shaders/simple_framebuffer.vs", "./resources/shaders/simple_framebuffer.fs");
        add_shader("grid", "./resources/shaders/grid.vs", "./resources/shaders/grid.fs");
        auto model_id = get_mutable_shader("model")->get_id();
        auto animation_id = get_mutable_shader("animation")->get_id();
        auto grid_id = get_mutable_shader("grid")->get_id();
        unsigned int uniform_block_id_model = glGetUniformBlockIndex(model_id, "Matrices");
        unsigned int uniform_block_id_animation = glGetUniformBlockIndex(animation_id, "Matrices");
        unsigned int uniform_block_id_grid = glGetUniformBlockIndex(grid_id, "Matrices");

        glUniformBlockBinding(model_id, uniform_block_id_model, 0);
        glUniformBlockBinding(grid_id, uniform_block_id_grid, 0);
        glUniformBlockBinding(animation_id, uniform_block_id_animation, 0);

        glGenBuffers(1, &matrices_UBO_);
        glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO_);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrices_UBO_, 0, 2 * sizeof(glm::mat4));
    }
    void SharedResources::set_ubo_projection(const glm::mat4 &projection)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO_);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void SharedResources::set_ubo_view(const glm::mat4 &view)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, matrices_UBO_);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void SharedResources::set_dt(float dt)
    {
        dt_ = dt;
    }
}

namespace anim
{
    void SharedResources::convert_to_entity(std::shared_ptr<Entity> &entity,
                                            std::shared_ptr<Model> &model,
                                            const std::shared_ptr<ModelNode> &model_node,
                                            Entity *parent_entity)
    {
        if (!model_node)
        {
            return;
        }
        int child_num = model_node->childrens.size();
        const std::string &name = model_node->name;

        entity.reset(new Entity(name, single_entity_list_.size(), parent_entity));
        single_entity_list_.push_back(entity);

        // mesh component
        if (model_node->meshes.size() != 0)
        {
            auto mesh = entity->add_component<MeshComponent>();
            mesh->set_meshes(model_node->meshes);
            mesh->set_shader(shaders_["model"]);
            mesh->set_entity(entity.get());
            if (model_node->has_bone)
            {
                mesh->set_shader(shaders_["animation"]);
            }
        }
        // find mising bone
        ArmatureComponent *parent_armature = (parent_entity) ? parent_entity->get_component<ArmatureComponent>() : nullptr;
        auto bone_info = model->get_pointer_bone_info(name);
        auto bind_pose_transformation = model_node->relative_transformation;
        PoseComponent *parent_pose = (parent_armature) ? parent_armature->get_pose() : nullptr;
        if (parent_armature && !bone_info)
        {
            auto parent_offset = parent_armature->get_bone_offset();
            BoneInfo missing_bone_info{
                model->bone_count_++,
                glm::inverse(glm::inverse(parent_offset) * bind_pose_transformation)};
            parent_pose->add_bone(name, missing_bone_info);
            model->bone_info_map_[name] = missing_bone_info;
            bone_info = &(model->bone_info_map_[name]);
        }
        // armature component
        if (bone_info)
        {
            PoseComponent *pose = parent_pose;
            auto armature = entity->add_component<ArmatureComponent>();
            if (parent_armature)
            {
                glm::vec4 relative_pos = bind_pose_transformation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                pose = parent_armature->get_pose();
                parent_armature->set_local_scale(child_num, glm::distance(relative_pos, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
                parent_armature->set_local_rotation_with_target(child_num, glm::vec3(relative_pos));
                armature->set_local_scale(0, parent_armature->get_local_scale());
                armature->set_local_rotation(0, parent_armature->get_local_rotation());
            }
            else
            {
                auto animation = parent_entity->add_component<AnimationComponent>();
                pose = parent_entity->add_component<PoseComponent>();
                pose->set_bone_info_map(model->bone_info_map_);
                pose->set_animator(animator_.get());
                pose->set_shader(shaders_["animation"].get());
                pose->set_animation_component(animation);
                pose->set_armature_root(entity.get());
                pose->set_dt(&dt_);
                armature->set_local_scale(0, 100.0f);
            }
            armature->set_pose(pose);
            armature->set_entity(entity.get());
            armature->set_name(name);
            armature->set_bone_id(bone_info->id);
            armature->set_bone_offset(bone_info->offset);
            armature->set_shader(shaders_["animation"].get());
        }
        auto &children = entity->get_mutable_children();
        children.resize(child_num);
        for (int i = 0; i < child_num; i++)
        {
            convert_to_entity(children[i], model, model_node->childrens[i], entity.get());
        }
    }
}