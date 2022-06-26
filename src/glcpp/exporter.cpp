#include "exporter.h"
#include "framebuffer.h"
#include "model.h"
#include "utility.hpp"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <json/json.h>

#include <fstream>
#include <set>
#include <map>

namespace glcpp
{
    void Exporter::to_png(Framebuffer *framebuffer, const char *save_path)
    {
        auto format = framebuffer->get_format();
        auto width = framebuffer->get_width();
        auto height = framebuffer->get_height();
        auto FBO = framebuffer->get_fbo();

        stbi_flip_vertically_on_write(true);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, width, height);
        size_t size = static_cast<size_t>(width * height);
        GLubyte *pixels = (GLubyte *)malloc(size * sizeof(GLubyte) * 4u);
        glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);

        if (format == GL_RGBA)
            stbi_write_png(save_path, width, height, 4, pixels, 0);
        else if (format == GL_RGB)
            stbi_write_png(save_path, width, height, 3, pixels, 0);

        free(pixels);
    }
    void Exporter::to_json(Model *model, const char *save_path)
    {
        Json::Value result_json;
        result_json["node"] = dfs(model->get_mutable_root_node().get(), "");
        result_json["name"] = "model";
        std::ofstream json_stream(save_path, std::ofstream::binary);
        json_stream << result_json.toStyledString();
        if (json_stream.is_open())
        {
            json_stream.close();
        }
    }
    Json::Value Exporter::dfs(ModelNode *node, const std::string &parent_name)
    {
        Json::Value node_json;
        auto &transform = node->initial_transformation;
        const auto &[t, r, s] = glcpp::DecomposeTransform(transform);
        node_json["name"] = node->name;
        node_json["position"] = get_vec_json(t);
        node_json["rotation"] = get_quat_json(r);
        node_json["scale"] = get_vec_json(s);
        node_json["parent_name"] = parent_name;
        node_json["child"] = Json::arrayValue;
        for (auto &sub_node : node->childrens)
        {
            node_json["child"].append(dfs(sub_node.get(), node->name));
        }
        return node_json;
    }
    Json::Value Exporter::get_quat_json(const glm::quat &r)
    {
        Json::Value ret_json;
        ret_json["w"] = r.w;
        ret_json["x"] = r.x;
        ret_json["y"] = r.y;
        ret_json["z"] = r.z;
        return ret_json;
    }
    Json::Value Exporter::get_vec_json(const glm::vec3 &p)
    {
        Json::Value ret_json;
        ret_json["x"] = p.x;
        ret_json["y"] = p.y;
        ret_json["z"] = p.z;
        return ret_json;
    }
}