#include "application.h"
#include "window.h"
#include "model.h"
#include "camera.h"
#include "framebuffer.h"
#include "cubemap.h"
#include "shader.h"

namespace glcpp
{
    Application *Application::instance_ = 0;
    bool Application::init_instance()
    {
        if (instance_ != nullptr)
        {
            return false;
        }
        instance_ = new Application();
        return true;
    }
    Application::~Application()
    {
        window_.reset();
        cubemap_.reset();
        camera_.reset();
        framebuffers_.clear();
        models_.clear();
        shader_.clear();

        destroy_instance();
    }
    void Application::destroy_instance()
    {
        if (instance_ != nullptr)
        {
            delete[] instance_;
            instance_ = nullptr;
        }
    }
    Application *Application::get_instance()
    {
        if (instance_ == nullptr)
        {
            if (!init_instance())
            {
                throw std::runtime_error("faild to get Instance");
            }
        }
        return instance_;
    }
    void Application::loop()
    {
        while (true)
        {
            std::cout << "ddd";
        }
    }
}