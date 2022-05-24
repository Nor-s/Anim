#ifndef GLCPP_APPLICATION_H
#define GLCPP_APPLICATION_H

#include <memory>
#include <vector>

namespace glcpp
{
    class Shader;
    class Window;
    class Cubemap;
    class Camera;
    class Framebuffer;
    class Model;
    template <typename T>
    class Application
    {
    protected:
        static bool
        init_instance()
        {
            if (instance_ != nullptr)
            {
                return false;
            }
            instance_ = new T();
            return true;
        }
        virtual ~Application()
        {
            window_.reset();
        }

    public:
        static void destroy_instance()
        {
            if (instance_ != nullptr)
            {
                delete instance_;
                instance_ = nullptr;
            }
        }
        static T *get_instance()
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
        virtual void loop()
        {
        }
        virtual void init(int width, int height, const std::string &title)
        {
            window_.reset(new Window(width, height, title));
        }

    protected:
        Application(){};
        inline static T *instance_ = nullptr;
        std::unique_ptr<Window> window_;
    };
}

#endif