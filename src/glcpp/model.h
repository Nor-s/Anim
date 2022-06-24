#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <map>

#include "component/transform_component.h"

#include "mesh.h"

namespace glcpp
{
    class Shader;
    // can't load "../"
    unsigned int TextureFromFile(const char *path, const std::filesystem::path &directory, const aiScene *scene);
    void LoadMemory(const aiTexture *texture, unsigned int *id);
    struct BoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;

        const glm::mat4 &get_offset() const
        {
            return offset;
        }
        const int &get_id() const
        {
            return id;
        }
    };
    struct ModelNode
    {
        glm::mat4 initial_transformation;
        TransformComponent relative_transformation;
        std::string name;
        std::vector<std::shared_ptr<ModelNode>> childrens;
        ModelNode(const glm::mat4 &initial_transform, const TransformComponent &relative_transform, const std::string &node_name, unsigned int num_children)
            : initial_transformation(initial_transform), relative_transformation(relative_transform), name(node_name)
        {
            childrens.resize(num_children);
        }
        ~ModelNode()
        {
            childrens.clear();
        }
        glm::mat4 get_mix_transformation() const
        {
            return initial_transformation * relative_transformation.get_mat4();
        }
    };

    /**
     * load_model => process_node => process_mesh(mesh... vertices, indices, textures...)
     */
    class Model
    {
    public:
        Model(const char *path);
        Model(const char *path, const aiScene *scene);
        ~Model();

        void draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection);
        void draw(Shader &shader);

        TransformComponent &get_mutable_transform();

        const std::map<std::string, BoneInfo> &get_bone_info_map() const;
        std::map<std::string, BoneInfo> &get_mutable_bone_info_map();
        int &get_mutable_bone_count();
        const ModelNode *get_root_node() const;
        std::shared_ptr<ModelNode> &get_mutable_root_node();
        void get_ai_node_for_anim(aiNode *ai_node, ModelNode *model_node, aiNode *parent_ai_node);
        void get_ai_root_node_for_anim(aiNode *ai_node);

    private:
        // heirarchy
        std::shared_ptr<ModelNode> root_node_;
        // model data
        std::vector<Mesh> meshes_;
        std::filesystem::path directory_;
        std::vector<Texture> textures_loaded_;
        TransformComponent *transform_;
        // bone data
        std::map<std::string, BoneInfo> bone_info_map_;
        int bone_count_ = 0;
        int node_count_ = 0;

        void load_model(const char *path);
        void load_model(const char *path, const aiScene *scene);
        /**
         * @brief 루트노드를 처음에 입력받아, 자식노드들을 순회하면서 메쉬들을 찾음
         *
         * @param node
         * @param scene
         */
        void process_node(std::shared_ptr<ModelNode> &model_node, aiNode *ai_node, const aiScene *scene);
        /**
         * @brief 모든 vertex 데이터를 얻고, mesh의 indices를 얻고, 연관된 material(texture) 데이터를 얻는다.
         *
         * @param mesh
         * @param scene
         * @return Mesh
         */
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        /**
         * @brief bone_info_map_에 bone 데이터를 넣고, vertex에 영향을 주는 bone과 그의 영향(가중치)을 집어넣음
         *
         * @param mesh
         * @param scene
         * @param vertices
         */
        void process_bone(aiMesh *mesh, const aiScene *scene, std::vector<Vertex> &vertices);

        std::vector<Texture> load_material_textures(aiMaterial *mat, const aiScene *scene, aiTextureType type,
                                                    std::string typeName);
    };
};

#endif