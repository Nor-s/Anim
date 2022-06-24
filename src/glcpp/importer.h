#ifndef SRC_GLCPP_IMPORTER_H
#define SRC_GLCPP_IMPORTER_H

#include <utility>
#include <memory>
#include <string>
#include <vector>

struct aiScene;

namespace glcpp
{
    class Model;
    class Animation;
    class Importer
    {
    public:
        Importer();
        ~Importer() = default;
        std::pair<std::shared_ptr<glcpp::Model>, std::vector<std::shared_ptr<glcpp::Animation>>> read_file(const char *path);

    private:
        void init_assimp_flag();
        std::shared_ptr<glcpp::Model> import_model(const aiScene *scene);
        std::vector<std::shared_ptr<glcpp::Animation>> import_animation(const aiScene *scene);

    private:
        unsigned int assimp_flag_ = 0U;
        std::string path_;
    };
}

#endif