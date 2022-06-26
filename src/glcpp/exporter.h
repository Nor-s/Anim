#ifndef SRC_GLCPP_EXPORTEER_H
#define SRC_GLCPP_EXPORTEER_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <json/json.h>

#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <string>

struct aiScene;

namespace glcpp
{
    class Model;
    struct ModelNode;
    class Animation;
    class Framebuffer;
    class Exporter
    {
    public:
        Exporter();
        ~Exporter() = default;
        void to_png(Framebuffer *framebuffer, const char *save_path);
        void to_json(Model *model, const char *save_path);
        // void to_glft2(const char *save_path);

    private:
        Json::Value dfs(ModelNode *node, const std::string &parent_name);
        Json::Value get_quat_json(const glm::quat &r);
        Json::Value get_vec_json(const glm::vec3 &p);
    };
}

#endif