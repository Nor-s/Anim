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

#include <tuple>
#include <map>
#include <memory>
#include <filesystem>

// add pixelate
namespace fs = std::filesystem;

class Scene4 : public Scene
{
public:
    Scene4(int width, int height)
    {
        init_option();
        init_skybox();
        init_shader();
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
        init_pixelate_framebuffer(width_, height_);
    }

    virtual void add_model(const char *file_name) override
    {
        model_.reset(new glcpp::Model{file_name});
    }
    virtual std::shared_ptr<glcpp::Model> &get_model() override
    {
        return model_;
    }
    virtual std::shared_ptr<glcpp::Framebuffer> &get_framebuffer() override
    {
        return framebuffer_;
    }
    virtual void pre_draw() override
    {
        update_framebuffer();
        set_view_and_projection();

        if (imgui_option_.get_flag(skyblur_flag_idx_))
        {
            capture_skybox();
        }
        pixelate_framebuffer_->pre_draw(model_, *model_shader_, view_, projection_);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        framebuffer_->bind_with_depth();
        {
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            if (imgui_option_.get_flag(skyblur_flag_idx_))
            {
                glDepthMask(false);
                skybox_framebuffer_->draw(*framebuffer_blur_shader_);
                glDepthMask(true);
            }
            else
            {
                skybox_->draw(view_, projection_);
            }
            pixelate_framebuffer_->draw();
        }
        framebuffer_->unbind();
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
            // imgui_option_.set_int_property(pixelate_value_idx_, std::tuple<std::string, int, int, int>{"pixelate value", 1, 1, (int)width_});
        }
        if (pixelate_framebuffer_->get_factor() != imgui_option_.get_int_property(pixelate_value_idx_))
        {
            init_pixelate_framebuffer(width_, height_);
        }
    }
    virtual void print_to_png(const std::string &file_name) override
    {
        pixelate_framebuffer_->print_to_png(file_name);
    }
    PixelateFramebuffer &get_pixelate_framebuffer()
    {
        return *pixelate_framebuffer_;
    }

private:
    void init_option()
    {
        skyblur_flag_idx_ = imgui_option_.push_flag("skybox_blur", true);
        pixelate_value_idx_ = imgui_option_.push_int_property("pixelate value", 1, 1, 64);
    }
    void init_skybox()
    {
        skybox_ = std::make_unique<glcpp::Cubemap>(skybox_faces_[0],
                                                   "./../../resources/shaders/skybox.vs",
                                                   "./../../resources/shaders/skybox.fs");
    }
    void init_pixelate_framebuffer(uint32_t width, uint32_t height)
    {
        int factor = imgui_option_.get_int_property(pixelate_value_idx_);
        pixelate_framebuffer_.reset(new PixelateFramebuffer(width / factor, height / factor));
        pixelate_framebuffer_->set_factor(factor);
        pixelate_framebuffer_->set_pixelate_shader(framebuffer_shader_);
        pixelate_framebuffer_->set_RGB_shader(framebuffer_shader_);
        pixelate_framebuffer_->set_tmp_shader(model_shader_);
    }
    void init_shader()
    {
        model_shader_ = std::make_unique<glcpp::Shader>("./../../resources/shaders/1.model_loading.vs", "./../../resources/shaders/1.model_loading.fs");
        framebuffer_shader_ = std::make_unique<glcpp::Shader>("./../../resources/shaders/simple_framebuffer.vs", "./../../resources/shaders/simple_framebuffer.fs");
        framebuffer_blur_shader_ = std::make_unique<glcpp::Shader>("./../../resources/shaders/simple_framebuffer.vs", "./../../resources/shaders/skybox_blur.fs");
    }
    void init_model()
    {
        add_model(fs::canonical(fs::path("./../../resources/models/nanosuit/nanosuit.obj")).string().c_str());
        model_->get_transform().set_translation(glm::vec3{0.0f, 0.0f, 0.0f}).set_rotation(glm::vec3{0.0f, 0.0f, 0.0f}).set_scale(glm::vec3{1.0f, 1.0f, 1.0f});
    }
    void init_camera()
    {
        camera_ = std::make_shared<glcpp::Camera>(glm::vec3(0.0f, 0.0f, 20.0f));
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
        projection_ = glm::perspective(glm::radians(camera_->Zoom), framebuffer_->get_aspect(), 0.1f, 10000.0f);
        view_ = camera_->GetViewMatrix();
    }

private:
    // skybox
    std::vector<std::string> skybox_faces_[6]{
        {"./../../resources/textures/skybox/right.jpg",
         "./../../resources/textures/skybox/left.jpg",
         "./../../resources/textures/skybox/top.jpg",
         "./../../resources/textures/skybox/bottom.jpg",
         "./../../resources/textures/skybox/front.jpg",
         "./../../resources/textures/skybox/back.jpg"},
        {"./../../resources/textures/cube/Bridge2/px.jpg",
         "./../../resources/textures/cube/Bridge2/nx.jpg",
         "./../../resources/textures/cube/Bridge2/py.jpg",
         "./../../resources/textures/cube/Bridge2/ny.jpg",
         "./../../resources/textures/cube/Bridge2/pz.jpg",
         "./../../resources/textures/cube/Bridge2/nz.jpg"},
        {"./../../resources/textures/cube/MilkyWay/px.jpg",
         "./../../resources/textures/cube/MilkyWay/nx.jpg",
         "./../../resources/textures/cube/MilkyWay/py.jpg",
         "./../../resources/textures/cube/MilkyWay/ny.jpg",
         "./../../resources/textures/cube/MilkyWay/pz.jpg",
         "./../../resources/textures/cube/MilkyWay/nz.jpg"},
        {"./../../resources/textures/cube/pisa/px.png",
         "./../../resources/textures/cube/pisa/nx.png",
         "./../../resources/textures/cube/pisa/py.png",
         "./../../resources/textures/cube/pisa/ny.png",
         "./../../resources/textures/cube/pisa/pz.png",
         "./../../resources/textures/cube/pisa/nz.png"},
        {"./../../resources/textures/cube/Park2/px.jpg",
         "./../../resources/textures/cube/Park2/nx.jpg",
         "./../../resources/textures/cube/Park2/py.jpg",
         "./../../resources/textures/cube/Park2/ny.jpg",
         "./../../resources/textures/cube/Park2/pz.jpg",
         "./../../resources/textures/cube/Park2/nz.jpg"},
        {"./../../resources/textures/cube/Tantolunden2/px.jpg",
         "./../../resources/textures/cube/Tantolunden2/nx.jpg",
         "./../../resources/textures/cube/Tantolunden2/py.jpg",
         "./../../resources/textures/cube/Tantolunden2/ny.jpg",
         "./../../resources/textures/cube/Tantolunden2/pz.jpg",
         "./../../resources/textures/cube/Tantolunden2/nz.jpg"}};

    std::unique_ptr<glcpp::Cubemap> skybox_;
    std::shared_ptr<glcpp::Model> model_;
    std::shared_ptr<glcpp::Shader> model_shader_;
    std::shared_ptr<glcpp::Shader> framebuffer_shader_;
    std::shared_ptr<glcpp::Shader> framebuffer_blur_shader_;
    std::shared_ptr<glcpp::Camera> camera_;
    std::shared_ptr<glcpp::Framebuffer> framebuffer_;
    std::shared_ptr<glcpp::Framebuffer> skybox_framebuffer_;
    std::shared_ptr<PixelateFramebuffer> pixelate_framebuffer_;
    glm::mat4 projection_ = glm::mat4(1.0f);
    glm::mat4 view_ = glm::mat4(1.0f);
    uint32_t width_ = 800;
    uint32_t height_ = 600;
    uint32_t skyblur_flag_idx_;
    uint32_t pixelate_value_idx_;
};

#endif