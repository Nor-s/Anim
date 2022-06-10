#ifndef SRC_SCENE_SCENE1_H
#define SRC_SCENE_SCENE1_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glcpp/cubemap.h"
#include "glcpp/shader.h"
#include "glcpp/model.h"
#include "glcpp/camera.h"
#include "glcpp/framebuffer.h"
#include "glcpp/transform_component.h"
#include "scene.hpp"
#include "pixelate_framebuffer.h"
#include "glcpp/anim/animator.hpp"
#include "glcpp/utility.hpp"
#include <tuple>
#include <map>
#include <memory>
#include <filesystem>

#include <assimp/Exporter.hpp>

namespace fs = std::filesystem;

class Scene1 : public Scene
{
public:
    Scene1(int width, int height)
    {
        init_option();
        init_skybox();
        init_shader();
        init_anim();
        init_model();
        init_framebuffer(width, height);
        init_camera();
    }
    virtual ~Scene1() = default;


    virtual void init_framebuffer(uint32_t width, uint32_t height) override
    {
        set_size(width, height);
        framebuffer_.reset(new glcpp::Framebuffer{width, height, GL_RGB});
        skybox_framebuffer_.reset(new glcpp::Framebuffer{width, height, GL_RGB});
        grid_framebuffer_.reset(new glcpp::Framebuffer{width, height, GL_RGB});
        init_pixelate_framebuffer(width_, height_);
    }

    // TODO: error with model_shader when model draw. maybe add_model logic to update framebuffer
    virtual void add_model(const char *file_name) override
    {
        models_.emplace_back(std::make_shared<glcpp::Model>(file_name));
        if (models_.back()->get_root_node() == nullptr) {
            models_.pop_back();
            return;
        }
        if (std::filesystem::path(std::filesystem::u8path(file_name)).extension() != ".obj")
        {
            animator_->add_animation(file_name);
            model_shader_.reset(new glcpp::Shader{"./resources/shaders/animation_loading.vs", "./resources/shaders/1.model_loading.fs"});
        }
        else
        {
            model_shader_.reset(new glcpp::Shader{"./resources/shaders/1.model_loading.vs", "./resources/shaders/1.model_loading.fs"});
        }
    }
    virtual std::shared_ptr<glcpp::Model> &get_model() override
    {
        return models_.back();
    }
    virtual std::shared_ptr<glcpp::Framebuffer> &get_framebuffer() override
    {
        return framebuffer_;
    }
    virtual void pre_draw() override
    {
        update_framebuffer();
        set_view_and_projection();
        update_flag_option();
        if (animator_)
        {
            animator_->update_animation(delta_time_, models_.back().get());

            model_shader_->use();
            auto &transforms = animator_->get_final_bone_matrices();
            int size = static_cast<int>(transforms.size());
            for (int i = 0; i < size; ++i)
                model_shader_->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }

        pixelate_framebuffer_->pre_draw(models_.back(), *model_shader_, view_, projection_);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
       
        grid_shader_->setMat4("view", view_);
        grid_shader_->setMat4("projection", projection_);
        grid_framebuffer_->draw(*grid_shader_);
 
      
        // TODO: glClearColor를 먼저 호출해야 framebuffer_에 적용되는 이유?
        framebuffer_->bind();
        {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

            grid_shader_->setMat4("view", view_);
            grid_shader_->setMat4("projection", projection_);
            grid_framebuffer_->draw(*grid_shader_); 
            if (imgui_option_.get_flag(skyblur_flag_idx_))
            {
               skybox_framebuffer_->draw(*framebuffer_blur_shader_);
                // skybox_->draw(view_, projection_);
            }
      

            pixelate_framebuffer_->draw();            

        }
        framebuffer_->unbind();
        auto error = glGetError();
        if (error != GL_NO_ERROR)
        {
#ifndef NDEBUG
            std::cout << error << std::endl;
#endif
        }
    }
    virtual void draw() override
    {
        framebuffer_->draw(*framebuffer_shader_);
    }
    virtual std::shared_ptr<glcpp::Camera> &get_camera() override
    {
        return camera_;
    }
    virtual void set_size(uint32_t width, uint32_t height) override
    {
        width_ = width;
        height_ = height;
    }
    void update_framebuffer()
    {
        if (framebuffer_->get_width() != width_ || framebuffer_->get_height() != height_)
        {
            init_framebuffer(width_, height_);
        }
        if (pixelate_framebuffer_->get_factor() != imgui_option_.get_int_property(pixelate_value_idx_))
        {
            init_pixelate_framebuffer(width_, height_);
        }
    }
    void update_flag_option()
    {
        if (imgui_option_.get_flag(skyblur_flag_idx_))
        {
            capture_skybox();
        }
        pixelate_framebuffer_->set_outline_flag(imgui_option_.get_flag(outline_flag_idx_));
        pixelate_framebuffer_->set_outline_color(imgui_option_.get_color3_property(outline_color_idx_));
    }
    virtual void print_to_png(const std::string &file_name) override
    {
        pixelate_framebuffer_->print_to_png(file_name);
    }
    PixelateFramebuffer &get_pixelate_framebuffer()
    {
        return *pixelate_framebuffer_;
    }
    void set_delta_time(float dt)
    {
        delta_time_ = dt;
    }
    virtual bool to_fbx(const std::string& file_name) override {
        Assimp::Exporter exporter;
        aiScene* scene = new aiScene();
        scene->mRootNode = new aiNode();
        models_.back()->get_ai_root_node_for_anim(scene->mRootNode);
        scene->mAnimations = new aiAnimation * [1];
        scene->mAnimations[0] = new aiAnimation();
        scene->mNumAnimations = 1;
        animator_->get_mutable_current_animation()->get_ai_animation(scene->mAnimations[0], scene->mRootNode);
        
        if (AI_SUCCESS == exporter.Export(scene, "gltf2", file_name.c_str())){
            return true;
        }

        std::cerr << exporter.GetErrorString() << std::endl;
        return false;
    }
private:
    void init_option()
    {
        outline_flag_idx_ = imgui_option_.push_flag("outline", true);
        skyblur_flag_idx_ = imgui_option_.push_flag("skybox blur", false);
        pixelate_value_idx_ = imgui_option_.push_int_property("pixelate", 1, 1, 64);
        outline_color_idx_ = imgui_option_.push_color3_property("outline", {0.0f, 0.0f, 0.0f});
    }
    void init_skybox()
    {
        skybox_ = std::make_unique<glcpp::Cubemap>(skybox_faces_[0],
                                                   "./resources/shaders/skybox.vs",
                                                   "./resources/shaders/skybox.fs");
    }
    void init_pixelate_framebuffer(uint32_t width, uint32_t height)
    {
        int factor = imgui_option_.get_int_property(pixelate_value_idx_);
        pixelate_framebuffer_.reset(new PixelateFramebuffer(width / factor, height / factor));
        pixelate_framebuffer_->set_factor(factor);
        pixelate_framebuffer_->set_pixelate_shader(framebuffer_shader_);
        pixelate_framebuffer_->set_RGB_shader(framebuffer_shader_);
        pixelate_framebuffer_->set_outline_shader(outline_shader_);
    }
    void init_shader()
    {
        model_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/animation_loading.vs", "./resources/shaders/1.model_loading.fs");
        framebuffer_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/simple_framebuffer.vs", "./resources/shaders/simple_framebuffer.fs");
        framebuffer_blur_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/simple_framebuffer.vs", "./resources/shaders/skybox_blur.fs");
        outline_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/outline_framebuffer.vs", "./resources/shaders/outline_framebuffer.fs");
        grid_shader_ = std::make_unique<glcpp::Shader>("./resources/shaders/grid.vs", "./resources/shaders/grid.fs");
    }
    void init_model()
    {
        add_model(fs::canonical(fs::path("./resources/models2/vampire/zom.fbx")).string().c_str());
        models_.back()->get_mutable_transform().set_translation(glm::vec3{0.0f, 0.0f, 0.0f}).set_rotation(glm::vec3{0.0f, 0.0f, 0.0f}).set_scale(glm::vec3{1.0f, 1.0f, 1.0f});
    }
    void init_anim()
    {
        animator_.reset(new glcpp::Animator{});
    }
    void init_camera()
    {
        camera_ = std::make_shared<glcpp::Camera>(glm::vec3(0.0f, 100.0f, 500.0f));
    }
    void capture_skybox()
    {
        glEnable(GL_DEPTH_TEST);
        skybox_framebuffer_->bind_with_depth();
        skybox_->draw(view_, projection_);
        skybox_framebuffer_->unbind();
    }
    void set_view_and_projection()
    {
        projection_ = glm::perspective(glm::radians(camera_->Zoom), framebuffer_->get_aspect(), 0.01f, 10000.0f);
        view_ = camera_->GetViewMatrix();
    }
    // TODO: !safe
    virtual glcpp::Animator *get_mutable_animator() override
    {
        return animator_.get();
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
    std::shared_ptr<glcpp::Shader> model_shader_;
    std::shared_ptr<glcpp::Shader> outline_shader_;
    std::shared_ptr<glcpp::Shader> framebuffer_shader_;
    std::shared_ptr<glcpp::Shader> framebuffer_blur_shader_;
    std::shared_ptr<glcpp::Shader> grid_shader_;
    std::shared_ptr<glcpp::Camera> camera_;
    std::shared_ptr<glcpp::Framebuffer> framebuffer_;
    std::shared_ptr<glcpp::Framebuffer> skybox_framebuffer_;
    std::shared_ptr<glcpp::Framebuffer> grid_framebuffer_;
    std::shared_ptr<PixelateFramebuffer> pixelate_framebuffer_;
    glm::mat4 projection_ = glm::mat4(1.0f);
    glm::mat4 view_ = glm::mat4(1.0f);
    uint32_t width_ = 800;
    uint32_t height_ = 600;
    uint32_t skyblur_flag_idx_;
    uint32_t pixelate_value_idx_;
    uint32_t outline_flag_idx_;
    uint32_t outline_color_idx_;

    std::shared_ptr<glcpp::Animator> animator_;
    float delta_time_ = 0.0f;
};

#endif