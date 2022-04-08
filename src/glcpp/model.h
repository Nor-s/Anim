#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <map>

#include "transform_component.h"

#include "mesh.h"
namespace glcpp
{
    class Shader;
    // can't load "../"
    unsigned int TextureFromFile(const char *path, const std::filesystem::path &directory);

    struct BoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;
    };

    /**
     * load_model => process_node => process_mesh(mesh... vertices, indices, textures...)
     */
    class Model
    {
    public:
        Model(const char *path);
        ~Model();

        void draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection);
        void draw(Shader &shader);

        WorldTransformComponent &get_mutable_transform();

        std::map<std::string, BoneInfo> &get_mutable_bone_info_map();
        int &get_mutable_bone_count();

    private:
        // model data
        std::vector<Mesh> meshes_;
        std::filesystem::path directory_;
        std::vector<Texture> textures_loaded_;
        WorldTransformComponent transform_;
        // bone data
        std::map<std::string, BoneInfo> bone_info_map_;
        int bone_count_ = 0;

        // bool gamma_correction;
        // int bone_count_;

        void load_model(const std::string &path);
        /**
         * @brief 루트노드를 처음에 입력받아, 자식노드들을 순회하면서 메쉬들을 찾음
         *
         * @param node
         * @param scene
         */
        void process_node(aiNode *node, const aiScene *scene);
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

        std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                                    std::string typeName);
    };
};

#endif