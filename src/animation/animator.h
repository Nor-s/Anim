#ifndef ANIM_ANIMATION_ANIMATOR_H
#define ANIM_ANIMATION_ANIMATOR_H

#include <glm/glm.hpp>
#include <vector>

namespace anim
{
    const unsigned int MAX_BONE_NUM = 128;

    class Model;
    class Shader;
    class Animation;
    class Entity;
    class AnimationComponent;

    class Animator
    {
    public:
        Animator();
        void update_animation(float dt, AnimationComponent *animation, Entity *root, Shader *shader);
        void calculate_bone_transform(Entity *entity, Animation *animation, const glm::mat4 &parentTransform);

    private:
        std::vector<glm::mat4> final_bone_matrices_;
        float current_time_{0.0f};
        float factor_{1.0f};
    };
}
#endif
