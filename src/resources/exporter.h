#ifndef SRC_ANIM_RESOURCES_EXPORTEER_H
#define SRC_ANIM_RESOURCES_EXPORTEER_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <json/json.h>

#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <string>

struct aiScene;
struct aiNode;

namespace anim
{
    class Model;
    class Animation;
    class Framebuffer;
    class Entity;
    class Exporter
    {
    public:
        Exporter() = default;
        ~Exporter() = default;
        void to_png(Framebuffer *framebuffer, const char *save_path);
        void to_json(Entity *entity, const char *save_path);
        void to_glft2(Entity *entity, const char *save_path, const char *model_path);

    private:
        Json::Value dfs(Entity *node, const std::string &parent_name);
        Json::Value get_quat_json(const glm::quat &r);
        Json::Value get_vec_json(const glm::vec3 &p);

        void to_ai_node(aiNode *ai_node, Entity *entity, aiNode *parent_ai_node = nullptr);
    };
}

#endif