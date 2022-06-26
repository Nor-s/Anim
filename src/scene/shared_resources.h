#ifndef SRC_SCENE_SHARED_RESOURCES_H
#define SRC_SCENE_SHARED_RESOURCES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glcpp/cubemap.h"
#include "glcpp/shader.h"
#include "glcpp/model.h"
#include "glcpp/component/transform_component.h"
#include "glcpp/anim/animator.hpp"
#include "glcpp/utility.hpp"
#include "glcpp/importer.h"
#include <tuple>
#include <map>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

class SharedResources
{

public:
    SharedResources()
    {
        init_skybox();
        init_shader();
        init_anim();
        init_model();
    }
    virtual ~SharedResources() = default;
    glcpp::Cubemap *get_mutable_skybox()
    {
        return skybox_.get();
    }
    std::shared_ptr<glcpp::Model> &get_model(int idx)
    {
        return models_[idx];
    }
    void add_model_and_animation(const char *path)
    {
        glcpp::Importer importer;
        auto [sp_model, sp_animations] = importer.read_file(path);
        if (sp_model)
        {
            models_.push_back(sp_model);
        }
        if (sp_animations.size() > 0)
        {
            animations_.insert(animations_.end(), sp_animations.begin(), sp_animations.end());
        }
    }
    void init_skybox()
    {
        skybox_ = std::make_unique<glcpp::Cubemap>(skybox_faces_[0],
                                                   "./resources/shaders/skybox.vs",
                                                   "./resources/shaders/skybox.fs");
    }
    void init_shader()
    {
        anim_model_shader_.reset(new glcpp::Shader{"./resources/shaders/animation_loading.vs", "./resources/shaders/1.model_loading.fs"});
        obj_model_shader_.reset(new glcpp::Shader{"./resources/shaders/1.model_loading.vs", "./resources/shaders/1.model_loading.fs"});
        framebuffer_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/simple_framebuffer.vs", "./resources/shaders/simple_framebuffer.fs");
        framebuffer_blur_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/simple_framebuffer.vs", "./resources/shaders/skybox_blur.fs");
        outline_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/outline_framebuffer.vs", "./resources/shaders/outline_framebuffer.fs");
        grid_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/grid.vs", "./resources/shaders/grid.fs");
    }
    void init_model()
    {
        // add_model(fs::canonical(fs::path("./resources/models2/vampire/zom.fbx")).string().c_str());
        models_.back()->get_mutable_transform().set_translation(glm::vec3{0.0f, 0.0f, 0.0f}).set_rotation(glm::vec3{0.0f, 0.0f, 0.0f}).set_scale(glm::vec3{1.0f, 1.0f, 1.0f});
    }
    void init_anim()
    {
    }

private:
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
    std::vector<std::shared_ptr<glcpp::Model>> models_;
    std::vector<std::shared_ptr<glcpp::Animation>> animations_;

    std::shared_ptr<glcpp::Shader> obj_model_shader_;
    std::shared_ptr<glcpp::Shader> anim_model_shader_;
    std::shared_ptr<glcpp::Shader> outline_shader_;
    std::shared_ptr<glcpp::Shader> framebuffer_shader_;
    std::shared_ptr<glcpp::Shader> framebuffer_blur_shader_;
    std::shared_ptr<glcpp::Shader> grid_shader_;
};

#endif