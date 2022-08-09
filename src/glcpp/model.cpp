#include "model.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

#include "shader.h"
#include "utility.hpp"
#include "component/animation_component.h"

namespace glcpp
{
    Model::Model(const char *path, const aiScene *scene)
    {
        load_model(path, scene);
    }

    Model::Model(const char *path)
    {
        load_model(path);
    }

    Model::~Model()
    {
        root_node_.reset();
    }

    void Model::load_model(const char *path)
    {
        Assimp::Importer import;
        unsigned int assimp_read_flag = aiProcess_Triangulate |
                                        aiProcess_GenUVCoords |
                                        aiProcess_OptimizeMeshes |
                                        aiProcess_ValidateDataStructure |
                                        aiProcess_GenNormals |
                                        aiProcess_CalcTangentSpace;
        assimp_read_flag |= aiProcess_LimitBoneWeights;
        assimp_read_flag |= aiProcess_JoinIdenticalVertices;

        import.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        const aiScene *scene = import.ReadFile(path, assimp_read_flag);

        // aiProcess_MakeLeftHanded;
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            root_node_ = nullptr;

#ifndef NDEBUG
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
#endif
            return;
        }
        load_model(path, scene);
    }

    void Model::load_model(const char *path, const aiScene *scene)
    {
        directory_ = std::filesystem::u8path(path);
        name_ = directory_.filename().string();
        process_node(root_node_, scene->mRootNode, scene);
        textures_loaded_.clear();
        process_armature(root_node_, armature_, nullptr);
        printf("END_loadmodel\n");
    }

    void Model::process_node(std::shared_ptr<ModelNode> &model_node, aiNode *ai_node, const aiScene *scene)
    {
        node_count_++;
        std::string model_name = std::string(ai_node->mName.C_Str());
        model_name = model_name.substr(model_name.find_last_of(':') + 1);
        auto find_mixamorig = model_name.find("mixamorig");
        if (find_mixamorig != std::string::npos)
        {
            model_name = model_name.substr(find_mixamorig + 9);
        }
        model_node.reset(new ModelNode(AiMatToGlmMat(ai_node->mTransformation),
                                       model_name,
                                       ai_node->mNumChildren));

        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[ai_node->mMeshes[i]];
            meshes_.emplace_back(process_mesh(mesh, scene));
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
        {
            process_node(model_node->childrens[i], ai_node->mChildren[i], scene);
        }
    }

    void Model::get_ai_node_for_anim(aiNode *ai_node, ModelNode *model_node, aiNode *parent_ai_node)
    {
        ai_node->mName = aiString(model_node->name.c_str());
        ai_node->mTransformation = GlmMatToAiMat(model_node->initial_transformation);
        ai_node->mParent = parent_ai_node;
        ai_node->mNumChildren = model_node->childrens.size();
        ai_node->mChildren = new aiNode *[ai_node->mNumChildren];

        for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
        {
            ai_node->mChildren[i] = new aiNode();
            get_ai_node_for_anim(ai_node->mChildren[i], model_node->childrens[i].get(), ai_node);
        }
    }

    void Model::get_ai_root_node_for_anim(aiNode *ai_root_node)
    {
        get_ai_node_for_anim(ai_root_node, root_node_.get(), NULL);
    }

    Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        MaterialProperties mat_properties;
#ifndef NDEBUG
        std::cout << mesh->mName.C_Str() << std::endl;
#endif

        // process vertex
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.set_position(AiVecToGlmVec(mesh->mVertices[i]));
            if (mesh->mNormals)
            {
                vertex.set_normal(AiVecToGlmVec(mesh->mNormals[i]));
            }
            if (mesh->mTextureCoords[0])
            {
                vertex.set_texture_coords(glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            }
            if (mesh->mTangents)
            {
                vertex.set_tangent(glm::vec3{
                    mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z});
            }
            if (mesh->mBitangents)
            {
                vertex.set_bitangent(glm::vec3{
                    mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z});
            }

            vertices.push_back(vertex);
        }

        process_bone(mesh, scene, vertices);

        // process indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // process material
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D color{};
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        mat_properties.ambient.x = color.r;
        mat_properties.ambient.y = color.g;
        mat_properties.ambient.z = color.b;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mat_properties.diffuse.x = color.r;
        mat_properties.diffuse.y = color.g;
        mat_properties.diffuse.z = color.b;

        std::cout << mat_properties.diffuse.x << " " << mat_properties.diffuse.y << " " << mat_properties.diffuse.z << std::endl;
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        mat_properties.specular.x = color.r;
        mat_properties.specular.y = color.g;
        mat_properties.specular.z = color.b;
        float shininess{};
        material->Get(AI_MATKEY_SHININESS, shininess);
        mat_properties.shininess = shininess;

        std::vector<Texture> diffuseMaps = load_material_textures(material, scene, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        if (diffuseMaps.size() > 0)
        {
            mat_properties.has_diffuse_texture = true;
        }

        std::vector<Texture> specularMaps = load_material_textures(material, scene, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = load_material_textures(material, scene, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = load_material_textures(material, scene, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures, mat_properties);
    }

    void Model::process_bone(aiMesh *mesh, const aiScene *scene, std::vector<Vertex> &vertices)
    {
        auto &bone_info_map = bone_info_map_;
        int &bone_count = bone_count_;
        int bone_num = mesh->mNumBones;
        for (int bone_idx = 0; bone_idx < bone_num; ++bone_idx)
        {
            int bone_id = -1;
            std::string bone_name = mesh->mBones[bone_idx]->mName.C_Str();
            bone_name = bone_name.substr(bone_name.find_last_of(':') + 1);
            auto find_mixamorig = bone_name.find("mixamorig");
            if (find_mixamorig != std::string::npos)
            {
                bone_name = bone_name.substr(find_mixamorig + 9);
            }
            if (bone_info_map.find(bone_name) == bone_info_map.end())
            {
                BoneInfo new_bone_info{};
                new_bone_info.id = bone_count;
                new_bone_info.offset = AiMatToGlmMat(mesh->mBones[bone_idx]->mOffsetMatrix);
                bone_info_map[bone_name] = new_bone_info;
                bone_id = bone_count;
                bone_count++;
                std::cout << ("bone_name: " + bone_name + " bone_id: " + std::to_string(bone_id)) << "\n";
            }
            else
            {
                bone_id = bone_info_map[bone_name].id;
            }

            assert(bone_id != -1);

            auto weights = mesh->mBones[bone_idx]->mWeights;
            int weights_num = mesh->mBones[bone_idx]->mNumWeights;

            for (int weight_idx = 0; weight_idx < weights_num; ++weight_idx)
            {
                int vertex_id = weights[weight_idx].mVertexId;
                float weight = weights[weight_idx].mWeight;

                assert(static_cast<size_t>(vertex_id) <= vertices.size());
                vertices[vertex_id].set_bone(bone_id, weight);
            }
        }
    }

    std::vector<Texture> Model::load_material_textures(aiMaterial *mat, const aiScene *scene, aiTextureType type,
                                                       std::string typeName)
    {
#ifndef NDEBUG
        std::cout << "LoadMaterialTextures: " << (int)type << " " << mat->GetTextureCount(type) << std::endl;
#endif
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded_.size(); j++)
            {
                if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded_[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            { // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), directory_, scene);
                texture.type = typeName;
                texture.path = std::string(str.C_Str());
                textures.push_back(texture);
                textures_loaded_.push_back(texture); // add to loaded textures
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char *path, const std::filesystem::path &directory, const aiScene *scene)
    {
#ifndef NDEBUG
        std::cout << "TextureFromFile:: " << path << std::endl;
#endif
        std::string filename(path);
        size_t idx = filename.find_first_of("/\\");
        if (filename[0] == '.' || idx == 0)
        {
            filename = filename.substr(idx + 1);
        }
        std::filesystem::path tmp = directory;
        filename = tmp.replace_filename(filename).string();
        std::replace(filename.begin(), filename.end(), '\\', '/');

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
            if (auto texture = scene->GetEmbeddedTexture(path))
            {
                LoadMemory(texture, &textureID);
            }
            else
            {
#ifndef NDEBUG
                std::cout << "Texture failed to load at path: "
                          << ":" << path << "\n";
#endif
            }
        }
        stbi_image_free(data);

        return textureID;
    }

    void LoadMemory(const aiTexture *texture, unsigned int *id)
    {
#ifndef NDEBUG
        std::cout << "LoadMemory" << std::endl;
#endif
        if (!*id)
            glGenTextures(1, id);
        glBindTexture(GL_TEXTURE_2D, *id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        unsigned char *image_data = nullptr;
        int width = 0, height = 0, components_per_pixel;
        if (texture->mHeight == 0)
        {
            image_data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(texture->pcData),
                                               texture->mWidth,
                                               &width,
                                               &height,
                                               &components_per_pixel,
                                               0);
        }
        else
        {
            image_data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(texture->pcData),
                                               texture->mWidth * texture->mHeight,
                                               &width,
                                               &height,
                                               &components_per_pixel, 0);
        }

        if (components_per_pixel == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        }
        else if (components_per_pixel == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image_data);
    }
}
namespace glcpp
{
    glm::mat4 tmp_mat = glm::mat4(1.0);
    void Model::draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &transform)
    {
        shader.use();
        shader.set_mat4("armature", glm::mat4(1.0f));
        shader.set_mat4("projection", projection);
        shader.set_mat4("view", view);
        shader.set_mat4("model", transform);
        tmp_mat = transform;
        // draw(shader);
        shader.set_mat4("model", glm::translate(transform, glm::vec3(200.0f, 0.0f, 000.0f)));
        draw(shader);
        draw_armature(*armature_, shader, view, projection, glm::mat4(1.0));
    }
    void Model::draw_armature(ArmatureNode &armature, Shader &shader, const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &transform)
    {
        glm::mat4 world = transform * armature.initial_transformation;
        glm::mat4 tmp = glm::scale(world, glm::vec3(armature.get_scale()));
        shader.set_mat4("armature", tmp);
        armature.armature->draw(shader, view, projection, tmp_mat);

        for (int i = 0; i < armature.childrens.size(); i++)
        {
            if (armature.childrens[i])
            {
                draw_armature(*(armature.childrens[i]), shader, view, projection, world);
            }
        }
    }

    void Model::draw(Shader &shader)
    {
        for (unsigned int i = 0; i < meshes_.size(); i++)
            meshes_[i].draw(shader);
    }

    std::map<std::string, BoneInfo> &Model::get_mutable_bone_info_map()
    {
        return bone_info_map_;
    }

    const std::map<std::string, BoneInfo> &Model::get_bone_info_map() const
    {
        return bone_info_map_;
    }

    const BoneInfo *Model::get_pointer_bone_info(const std::string &bone_name) const
    {
        auto it = bone_info_map_.find(bone_name);
        if (it != bone_info_map_.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    int &Model::get_mutable_bone_count()
    {
        return bone_count_;
    }

    const ModelNode *Model::get_root_node() const
    {
        return root_node_.get();
    }
    std::shared_ptr<ModelNode> &Model::get_mutable_root_node()
    {
        return root_node_;
    }
    ArmatureNode *Model::get_mutable_armature()
    {
        return armature_.get();
    }
    const std::string &Model::get_name() const
    {
        return name_;
    }
}

namespace glcpp
{
    // process only one armature
    void Model::process_armature(const std::shared_ptr<ModelNode> &model_node, std::shared_ptr<ArmatureNode> &armature, ArmatureNode *parent_armature)
    {
        unsigned int child_size = model_node->childrens.size();
        auto bone = bone_info_map_.find(model_node->name);

        if (parent_armature && bone == bone_info_map_.end())
        {
            return;
        }
        if (bone != bone_info_map_.end())
        {
            printf("begin-----%s (%d)\n", bone->first.c_str(), bone->second.id);
            armature.reset(new ArmatureNode(model_node->initial_transformation, model_node->name, bone->second.id));
            if (parent_armature)
            {
                glm::vec4 world = armature->initial_transformation * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                parent_armature->set_scale(glm::distance(world, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
                armature->set_scale(parent_armature->get_scale());
            }
            printf("end-----%s\n", bone->first.c_str());
        }
        for (int i = 0; i < child_size; i++)
        {
            if (armature)
            {
                std::shared_ptr<ArmatureNode> arm = nullptr;
                process_armature(model_node->childrens[i], arm, armature.get());
                if (arm && armature)
                {
                    armature->childrens.push_back(arm);
                }
            }
            else
            {
                process_armature(model_node->childrens[i], armature, parent_armature);
            }
        }
    }

}