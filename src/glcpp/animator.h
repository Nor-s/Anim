#ifndef GLCPP_ANIMATOR_H
#define GLCPP_ANIMATOR_H

#include <glm/glm.hpp>
#include <vector>

namespace glcpp
{
    const unsigned int MAX_BONE_NUM = 128;

    class Model;
    class Shader;
    class Animation;
    struct ModelNode;
    class Entity;

    class Animator
    {
    public:
        Animator();
        void update_animation(float dt, Entity *entity, Shader *shader);
        void calculate_bone_transform(const Model *model, const ModelNode *node, Animation *animation, glm::mat4 parentTransform);

    private:
        std::vector<glm::mat4> final_bone_matrices_;
        float current_time_{0.0f};
        float factor_{1.0f};
    };
}

#endif