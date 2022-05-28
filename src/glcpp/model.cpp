#include "model.h"

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

#include <stb/stb_image.h>

#include "shader.h"
#include "utility.hpp"

namespace glcpp
{
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
                                        aiProcess_SortByPType |
                                        aiProcess_GenUVCoords |
                                        aiProcess_OptimizeMeshes |
                                        aiProcess_ValidateDataStructure |
                                        //  aiProcess_ConvertToLeftHanded |
                                        // aiProcess_FlipUVs |
                                        aiProcess_GenNormals |
                                        aiProcess_CalcTangentSpace;
        assimp_read_flag |= aiProcess_LimitBoneWeights;
        assimp_read_flag |= aiProcess_JoinIdenticalVertices;
        assimp_read_flag |= aiProcess_FlipWindingOrder;
        import.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
        const aiScene *scene = import.ReadFile(path, assimp_read_flag);

        // aiProcess_MakeLeftHanded;
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
            return;
        }
        directory_ = std::filesystem::u8path(path);

        process_node(root_node_, scene->mRootNode, scene);
        transform_ = &(root_node_->relative_transformation);
    }
    void Model::process_node(std::shared_ptr<ModelNode> &model_node, aiNode *ai_node, const aiScene *scene)
    {
        node_count_++;
        std::string model_name = std::string(ai_node->mName.C_Str());
        model_node.reset(new ModelNode(AiMatToGlmMat(ai_node->mTransformation),
                                       TransformComponent(),
                                       model_name.substr(model_name.find_last_of(':') + 1),
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
    void Model::get_ai_node_for_anim(aiNode* ai_node, ModelNode* model_node, aiNode* parent_ai_node) {
        ai_node->mName = aiString(model_node->name.c_str());
        ai_node->mTransformation = GlmMatToAiMat(model_node->initial_transformation);
        ai_node->mParent = parent_ai_node;
        ai_node->mNumChildren = model_node->childrens.size();
        ai_node->mChildren = new aiNode*[ai_node->mNumChildren];
        for (int i = 0; i < ai_node->mNumChildren; i++) {
            ai_node->mChildren[i] = new aiNode();
            get_ai_node_for_anim(ai_node->mChildren[i], model_node->childrens[i].get(), ai_node);
        }
    }
    void Model::get_ai_root_node_for_anim(aiNode* ai_root_node) {
        get_ai_node_for_anim(ai_root_node, root_node_.get(), NULL);
    }

    Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // process vertex
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.set_position(AiVecToGlmVec(mesh->mVertices[i]));
            vertex.set_normal(AiVecToGlmVec(mesh->mNormals[i]));
            if (mesh->mTextureCoords[0])
            {
                vertex.set_texture_coords(glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
            }
            if (mesh->mTangents)
                vertex.set_tangent(glm::vec3{
                    mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z});
            if (mesh->mBitangents)
                vertex.set_bitangent(glm::vec3{
                    mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z});

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

        std::vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
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
            if (bone_info_map.find(bone_name) == bone_info_map.end())
            {
                BoneInfo new_bone_info;
                new_bone_info.id = bone_count;
                new_bone_info.offset = AiMatToGlmMat(mesh->mBones[bone_idx]->mOffsetMatrix);
                bone_info_map[bone_name] = new_bone_info;
                bone_id = bone_count;
                bone_count++;
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

    std::vector<Texture> Model::load_material_textures(aiMaterial *mat, aiTextureType type,
                                                       std::string typeName)
    {
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
                texture.id = TextureFromFile(str.C_Str(), directory_);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded_.push_back(texture); // add to loaded textures
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char *path, const std::filesystem::path &directory)
    {
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

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: "
                      << ":" << path << "\n";
            stbi_image_free(data);
        }

        return textureID;
    }
}
namespace glcpp
{
    void Model::draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection)
    {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", transform_->get_mat4());
        for (unsigned int i = 0; i < meshes_.size(); i++)
            meshes_[i].draw(shader);
    }

    void Model::draw(Shader &shader)
    {
        for (unsigned int i = 0; i < meshes_.size(); i++)
            meshes_[i].draw(shader);
    }

    TransformComponent &Model::get_mutable_transform()
    {
        return *transform_;
    }

    std::map<std::string, BoneInfo> &Model::get_mutable_bone_info_map()
    {
        return bone_info_map_;
    }

    const std::map<std::string, BoneInfo> &Model::get_bone_info_map() const
    {
        return bone_info_map_;
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
}