#ifndef SRC_SCENE_SHARED_RESOURCES_H
#define SRC_SCENE_SHARED_RESOURCES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tuple>
#include <map>
#include <memory>
#include <filesystem>
#include <vector>

namespace glcpp
{
    class Cubemap;
    class Model;
    class Animation;
    class Shader;
    class Animator;
}

class SharedResources
{

public:
    SharedResources();
    virtual ~SharedResources();
    glcpp::Cubemap *get_mutable_skybox();
    glcpp::Animator *get_mutable_animator();
    std::shared_ptr<glcpp::Model> get_mutable_model(int idx);
    std::shared_ptr<glcpp::Animation> get_mutable_animation(int idx);
    std::shared_ptr<glcpp::Model> back_mutable_model();
    std::shared_ptr<glcpp::Animation> back_mutable_animation();
    std::shared_ptr<glcpp::Shader> get_mutable_shader(const std::string &name);
    const std::vector<std::shared_ptr<glcpp::Model>> &get_models() const;
    const std::vector<std::shared_ptr<glcpp::Animation>> &get_animations() const;

    int get_models_size() const;
    int get_animations_size() const;
    std::pair<bool, bool> add_model_or_animation_by_path(const char *path);
    void add_shader(const std::string &name, const char *vs_path, const char *fs_path);

private:
    void init_skybox();
    void init_animator();
    // skybox
    std::vector<std::string> skybox_faces_[6]{
        {"./resources/textures/skybox/right.jpg",
         "./resources/textures/skybox/left.jpg",
         "./resources/textures/skybox/top.jpg",
         "./resources/textures/skybox/bottom.jpg",
         "./resources/textures/skybox/front.jpg",
         "./resources/textures/skybox/back.jpg"},
        {"./resources/textures/cube/Bridge2/px.jpg",
         "./resources/textures/cube/Bridge2/nx.jpg",
         "./resources/textures/cube/Bridge2/py.jpg",
         "./resources/textures/cube/Bridge2/ny.jpg",
         "./resources/textures/cube/Bridge2/pz.jpg",
         "./resources/textures/cube/Bridge2/nz.jpg"},
        {"./resources/textures/cube/MilkyWay/px.jpg",
         "./resources/textures/cube/MilkyWay/nx.jpg",
         "./resources/textures/cube/MilkyWay/py.jpg",
         "./resources/textures/cube/MilkyWay/ny.jpg",
         "./resources/textures/cube/MilkyWay/pz.jpg",
         "./resources/textures/cube/MilkyWay/nz.jpg"},
        {"./resources/textures/cube/pisa/px.png",
         "./resources/textures/cube/pisa/nx.png",
         "./resources/textures/cube/pisa/py.png",
         "./resources/textures/cube/pisa/ny.png",
         "./resources/textures/cube/pisa/pz.png",
         "./resources/textures/cube/pisa/nz.png"},
        {"./resources/textures/cube/Park2/px.jpg",
         "./resources/textures/cube/Park2/nx.jpg",
         "./resources/textures/cube/Park2/py.jpg",
         "./resources/textures/cube/Park2/ny.jpg",
         "./resources/textures/cube/Park2/pz.jpg",
         "./resources/textures/cube/Park2/nz.jpg"},
        {"./resources/textures/cube/Tantolunden2/px.jpg",
         "./resources/textures/cube/Tantolunden2/nx.jpg",
         "./resources/textures/cube/Tantolunden2/py.jpg",
         "./resources/textures/cube/Tantolunden2/ny.jpg",
         "./resources/textures/cube/Tantolunden2/pz.jpg",
         "./resources/textures/cube/Tantolunden2/nz.jpg"}};

    std::unique_ptr<glcpp::Cubemap> skybox_;
    std::unique_ptr<glcpp::Animator> animator_;
    std::vector<std::shared_ptr<glcpp::Model>> models_;
    std::vector<std::shared_ptr<glcpp::Animation>> animations_;
    std::map<std::string, std::shared_ptr<glcpp::Shader>> shaders_;
};

#endif