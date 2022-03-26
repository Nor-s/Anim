#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

namespace glcpp
{
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
    inline glm::vec3 get_glm_vec(const aiVector3D &vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
    }
    inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4 &from)
    {
        glm::mat4 to;
        // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1;
        to[1][0] = from.a2;
        to[2][0] = from.a3;
        to[3][0] = from.a4;
        to[0][1] = from.b1;
        to[1][1] = from.b2;
        to[2][1] = from.b3;
        to[3][1] = from.b4;
        to[0][2] = from.c1;
        to[1][2] = from.c2;
        to[2][2] = from.c3;
        to[3][2] = from.c4;
        to[0][3] = from.d1;
        to[1][3] = from.d2;
        to[2][3] = from.d3;
        to[3][3] = from.d4;
        return to;
    }
    inline glm::quat GetGLMQuat(const aiQuaternion &pOrientation)
    {
        return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
    }

    struct Bone
    {
        /*id is index in finalBoneMatrices*/
        int id;
        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;
    };

    class Model
    {
    public:
        Model(const char *path, bool gamma = false) : gamma_correction(gamma)
        {
            load_model(path);
        }
        void draw(Shader &shader)
        {
            for (unsigned int i = 0; i < meshes_.size(); i++)
                meshes_[i].draw(shader);
        }

        std::map<std::string, Bone> &get_bone_map()
        {
            return bone_map_;
        }

        int &get_bone_count()
        {
            return bone_count_;
        }

    private:
        // model data
        vector<Mesh> meshes_;
        string directory_;
        vector<Texture> textures_loaded_;
        bool gamma_correction;
        std::map<std::string, Bone> bone_map_;
        int bone_count_;

        void load_model(const string &path)
        {
            Assimp::Importer import;
            const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
                return;
            }
            directory_ = path.substr(0, path.find_last_of('/'));

            process_node(scene->mRootNode, scene);
        }
        void process_node(aiNode *node, const aiScene *scene)
        {
            std::cout << "---" << node->mName.C_Str() << "\n";
            // process all the node's meshes (if any)
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                meshes_.push_back(process_mesh(mesh, scene));
            }
            // then do the same for each of its children
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                process_node(node->mChildren[i], scene);
            }
        }
        /**
         * @brief 모든 vertex 데이터를 얻고, mesh의 indices를 얻고, 연관된 material(texture) 데이터를 얻는다.
         *
         * @param mesh
         * @param scene
         * @return Mesh
         */
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene)
        {
            vector<Vertex> vertices;
            vector<unsigned int> indices;
            vector<Texture> textures;

            // process vertex
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                set_bone_vertex_to_default(vertex);
                glm::vec3 vector;
                vertex.position = get_glm_vec(mesh->mVertices[i]);
                vertex.normal = get_glm_vec(mesh->mNormals[i]);
                // texcoord
                if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.tex_coords = vec;
                    /*
                    // tangent
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.tangent = vector;
                    // bitangent
                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.bitangent = vector;
                    */
                }
                else
                    vertex.tex_coords = glm::vec2(0.0f, 0.0f);
                vertices.push_back(vertex);
            }

            // process indices
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            // process material
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

            vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            std::vector<Texture> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            // ExtractBoneWeightForVertices(vertices, mesh, scene);
            cout << diffuseMaps.size() << "\n";

            return Mesh(vertices, indices, textures);
        }
        vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                               string typeName)
        {
            vector<Texture> textures;
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

        void set_bone_vertex_to_default(Vertex &vertex)
        {
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            {
                vertex.bone_ids[i] = -1;
                vertex.weights[i] = 0.0f;
            }
        }
        /*
                void SetVertexBoneData(Vertex &vertex, int boneID, float weight)
                {
                    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
                    {
                        if (vertex.m_BoneIDs[i] < 0)
                        {
                            vertex.m_Weights[i] = weight;
                            vertex.m_BoneIDs[i] = boneID;
                            break;
                        }
                    }
                }

                void ExtractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh, const aiScene *scene)
                {
                    auto &boneInfoMap = m_BoneInfoMap;
                    int &boneCount = m_BoneCounter;

                    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
                    {
                        int boneID = -1;
                        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
                        if (boneInfoMap.find(boneName) == boneInfoMap.end())
                        {
                            BoneInfo newBoneInfo;
                            newBoneInfo.id = boneCount;
                            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                            boneInfoMap[boneName] = newBoneInfo;
                            boneID = boneCount;
                            boneCount++;
                        }
                        else
                        {
                            boneID = boneInfoMap[boneName].id;
                        }
                        assert(boneID != -1);
                        auto weights = mesh->mBones[boneIndex]->mWeights;
                        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

                     // mesh의 bone들과 vertex를 연결
                        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
                        {
                            int vertexId = weights[weightIndex].mVertexId;
                            float weight = weights[weightIndex].mWeight;
                            assert(vertexId <= vertices.size());
                            SetVertexBoneData(vertices[vertexId], boneID, weight);
                        }
                    }
                }
                // checks all material textures of a given type and loads the textures if they're not loaded yet.
                // the required info is returned as a Texture struct.
                vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
                {
                    vector<Texture> textures;
                    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
                    {
                        aiString str;
                        mat->GetTexture(type, i, &str);
                        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                        bool skip = false;
                        for (unsigned int j = 0; j < textures_loaded.size(); j++)
                        {
                            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                            {
                                textures.push_back(textures_loaded[j]);
                                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                                break;
                            }
                        }
                        if (!skip)
                        { // if texture hasn't been loaded already, load it
                            Texture texture;
                            texture.id = TextureFromFile(str.C_Str(), this->directory);
                            texture.type = typeName;
                            texture.path = str.C_Str();
                            textures.push_back(texture);
                            textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                        }
                    }
                    return textures;
                }
                */
    };
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
    {
        string filename = string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            std::cout << filename << "\n";
            GLenum format;
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};

#endif