#ifndef ANIM_RESOURCES_SHARED_RESOURCES_H
#define ANIM_RESOURCES_SHARED_RESOURCES_H

#include "../graphics/mesh.h"
#include <memory>
#include <map>

namespace anim
{

    class Animator;
    class Shader;
    class Animation;
    class Entity;
    enum NodeType
    {

    };
    struct ModelResource
    {
    };
    struct AnimationResource
    {
        std::unique_ptr<Animation> animation;
        bool is_raw;
    };

    class SharedResources
    {
    public:
        SharedResources();
        ~SharedResources();
        Animator *get_mutable_animator();
        std::shared_ptr<Shader> get_mutable_shader(const std::string &name);

    private:
        void init_animator();
        std::unique_ptr<Animator> animator_;
        std::vector<std::unique_ptr<ModelResource>> models_;
        std::vector<std::unique_ptr<AnimationResource>> animations_;
        std::map<std::string, std::unique_ptr<Shader>> shaders_;
        std::vector<Entity> entities_;
        std::unique_ptr<Mesh> bone_;
    };

}
#endif