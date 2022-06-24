#include "importer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include "model.h"
#include "anim/json_animation.hpp"
#include "anim/assimp_animation.hpp"

namespace fs = std::filesystem;

namespace glcpp
{
    Importer::Importer()
    {
        init_assimp_flag();
    }

    std::pair<std::shared_ptr<glcpp::Model>, std::vector<std::shared_ptr<glcpp::Animation>>> Importer::read_file(const char *path)
    {
        std::shared_ptr<glcpp::Model> sp_model;
        std::vector<std::shared_ptr<glcpp::Animation>> sp_animations;
        path_ = std::string(path);

        fs::path fs_path = fs::u8path(path_);
        if (fs_path.extension() == ".json")
        {
            sp_animations.push_back(std::make_shared<glcpp::JsonAnimation>(path_.c_str()));
            return std::make_pair(sp_model, sp_animations);
        }
        try
        {
            Assimp::Importer importer;
            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
            const aiScene *scene = importer.ReadFile(path_.c_str(), assimp_flag_);
            sp_model = import_model(scene);
            sp_animations = import_animation(scene);
            if (sp_model == nullptr && sp_animations.size() == 0)
            {
#ifndef NDEBUG
                std::cout << "ERROR::IMPORTER: " << importer.GetErrorString() << std::endl;
#endif
            }
        }
        catch (std::exception &e)
        {
#ifndef NDEBUG
            std::cout << "ERROR::IMPORTER: " << e.what() << std::endl;
#endif
        }
        return std::make_pair(sp_model, sp_animations);
    }
    void Importer::init_assimp_flag()
    {
        assimp_flag_ = aiProcess_Triangulate |
                       aiProcess_GenUVCoords |
                       aiProcess_OptimizeMeshes |
                       aiProcess_ValidateDataStructure |
                       aiProcess_GenNormals |
                       aiProcess_CalcTangentSpace |
                       aiProcess_LimitBoneWeights |
                       aiProcess_JoinIdenticalVertices;
    }
    std::shared_ptr<glcpp::Model> Importer::import_model(const aiScene *scene)
    {
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            return nullptr;
        }
        return std::make_shared<glcpp::Model>(path_.c_str(), scene);
    }
    std::vector<std::shared_ptr<glcpp::Animation>> Importer::import_animation(const aiScene *scene)
    {
        std::vector<std::shared_ptr<glcpp::Animation>> sp_animations;
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            return sp_animations;
        }
        for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        {
            auto animation = scene->mAnimations[i];
            sp_animations.push_back(std::make_shared<glcpp::AssimpAnimation>(animation, scene));
        }
        return sp_animations;
    }
}