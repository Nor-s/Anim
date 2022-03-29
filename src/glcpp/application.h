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
    class Application;
    class Model;
    class Application
    {

    protected:
        static bool init_instance();

    public:
        virtual ~Application();
        static void destroy_instance();
        static Application *get_instance();
        void loop();

    private:
        static Application *instance_;
        Application() = default;
        std::unique_ptr<Window> window_;
        std::unique_ptr<Cubemap> cubemap_;
        std::unique_ptr<Camera> camera_;
        std::vector<std::unique_ptr<Framebuffer>> framebuffers_;
        std::vector<std::unique_ptr<Model>> models_;
        std::vector<std::unique_ptr<Shader>> shader_;
    };
}

#endif