#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

#include "transform_component.h"

#include "mesh.h"
namespace glcpp
{
    class Shader;
    unsigned int TextureFromFile(const char *path, const std::filesystem::path &directory);
    inline glm::vec3 GetGlmVec(const aiVector3D &vec);

    class Model
    {
    public:
        Model(const char *path);
        ~Model();

        void draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection);
        void draw(Shader &shader);

        WorldTransformComponent &get_transform();

    private:
        // model data
        std::vector<Mesh> meshes_;
        std::filesystem::path directory_;
        std::vector<Texture> textures_loaded_;
        WorldTransformComponent transform_;

        // bool gamma_correction;
        // int bone_count_;

        void load_model(const std::string &path);
        void process_node(aiNode *node, const aiScene *scene);
        /**
         * @brief 모든 vertex 데이터를 얻고, mesh의 indices를 얻고, 연관된 material(texture) 데이터를 얻는다.
         *
         * @param mesh
         * @param scene
         * @return Mesh
         */
        Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                                    std::string typeName);
    };
};

#endif