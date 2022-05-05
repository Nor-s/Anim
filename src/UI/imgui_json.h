#ifndef UI_IMGUI_JSON_H
#define UI_IMGUI_JSON_H

#include <json/json.h>
#include <glcpp/model.h>
#include <memory>
#include <glcpp/utility.hpp>
#include <fstream>
#include <set>
#include <map>

class ImguiJson
{
public:
    static Json::Value get_quat_json(const glm::quat &r)
    {
        Json::Value ret_json;
        ret_json["w"] = r.w;
        ret_json["x"] = r.x;
        ret_json["y"] = r.y;
        ret_json["z"] = r.z;
        return ret_json;
    }
    static Json::Value get_vec_json(const glm::vec3 &p)
    {
        Json::Value ret_json;
        ret_json["x"] = p.x;
        ret_json["y"] = p.y;
        ret_json["z"] = p.z;
        return ret_json;
    }
    static void dfs(std::shared_ptr<glcpp::ModelNode> &node, Json::Value &result_json, const std::string &parent_name)
    {
        Json::Value node_json;
        auto &transform = node->initial_transformation;
        const auto &[t, r, s] = glcpp::DecomposeTransform(transform);
        node_json["name"] = node->name;
        node_json["position"] = get_vec_json(t);
        node_json["rotation"] = get_quat_json(r);
        node_json["scale"] = get_vec_json(s);
        node_json["parent_name"] = parent_name;
        result_json["bindingPose"].append(node_json);
        for (auto &sub_node : node->childrens)
        {
            dfs(sub_node, result_json, node->name);
        }
    }
    static void ModelBindingPoseToJson(const std::string &file_name, glcpp::Model *model)
    {
        Json::Value result_json;
        dfs(model->get_mutable_root_node(), result_json, "");
        result_json["name"] = "model";
        std::ofstream json_stream(file_name.c_str(), std::ofstream::binary);
        json_stream << result_json.toStyledString();
        if (json_stream.is_open())
        {
            json_stream.close();
        }
    }
    static void AnimationToJson(const std::string &file_name, glcpp::Animation *anim)
    {
        auto &name_bone_map = anim->get_mutable_name_bone_map();
        Json::Value result_json;
        std::map<float, std::vector<glcpp::Bone *>> time_bones_map;
        for (auto &name_bone : name_bone_map)
        {
            auto &timelist = name_bone.second->get_mutable_time_list();
            for (auto time : timelist)
            {
                time_bones_map[time].push_back(name_bone.second.get());
            }
        }
        for (auto &time_bones : time_bones_map)
        {
            Json::Value frame_json;
            for (auto &bone : time_bones.second)
            {
                Json::Value bone_json;
                bone_json["name"] = bone->get_bone_name();
                glm::vec3 *p_pos = bone->get_mutable_pointer_positions(time_bones.first);
                glm::quat *p_quat = bone->get_mutable_pointer_rotations(time_bones.first);
                glm::vec3 *p_scale = bone->get_mutable_pointer_scales(time_bones.first);
                if (p_pos)
                {
                    bone_json["position"] = get_vec_json(*p_pos);
                }
                if (p_quat)
                {
                    bone_json["rotation"] = get_quat_json(*p_quat);
                }
                if (p_scale)
                {
                    bone_json["scale"] = get_vec_json(*p_scale);
                }
                frame_json["bones"].append(bone_json);
            }
            frame_json["time"] = time_bones.first;
            result_json["frames"].append(frame_json);
        }

        result_json["ticksPerSecond"] = anim->get_ticks_per_second();
        result_json["duration"] = anim->get_duration();
        result_json["fileName"] = anim->get_name();
        std::ofstream json_stream(file_name.c_str(), std::ofstream::binary);
        json_stream << result_json.toStyledString();
        if (json_stream.is_open())
        {
            json_stream.close();
        }
    }
};

#endif