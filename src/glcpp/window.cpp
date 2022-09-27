#include "window.h"
#include <iostream>
#include <iomanip> // for setw

namespace glcpp
{
    Window::Window(uint32_t width, uint32_t height, std::string title)
        : width_(width), height_(height), title_(title)
    {
        init_glfw();
    }
    Window::~Window()
    {
        if (handle_)
        {
#ifndef NDEBUG
            std::cout << "Window::~Window" << std::endl;
#endif
            destroy_window();
        }
    }
    bool Window::should_close()
    {
        return is_window_close_;
    }

    void Window::process_events()
    {
        SDL_PollEvent(&event_);
        switch (event_.type) {
                case SDL_QUIT:
                    is_window_close_ = true;
                    break;
        }
    }

    void Window::init_glfw()
    {
        #ifndef NDEBUG
        std::cout<<"Window::init_SDL START"<<std::endl;
        #endif
        SDL_SetMainReady();
        // INITIALIZE SDL:
        if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) < 0) 
        {
            throw(std::string("Failed to initialize SDL: ") + SDL_GetError());
        }
        // CONFIGURE OPENGL ATTRIBUTES USING SDL:
        int context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#ifndef NDEBUG
        context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        // CREATE AND SDL WINDOW CONFIGURED FOR OPENGL:
        if (0 == (handle_ = SDL_CreateWindow("OpenGL Init Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, window_flags))) {
            throw(std::string("Failed to create window: ") + SDL_GetError());
        }
        // CREATE THE OPENGL CONTEXT AND MAKE IT CURRENT:
        if(NULL == (gl_context_ = SDL_GL_CreateContext(handle_))) {
            throw(std::string("Failed to create OpenGL context"));
        }        
        else {
            SDL_GL_MakeCurrent(handle_, gl_context_);
        }
        SDL_GL_SetSwapInterval(1); // Enable vsync
        // glad: load all OpenGL function pointers
        // ---------------------------------------
        #ifndef NDEBUG
        std::cout<<"Window::init_GLAD START"<<std::endl;
        #endif
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }
        #ifndef NDEBUG
        std::cout<<"Window::init_GLAD END"<<std::endl;
        #endif
        // LOG OPENGL VERSION, VENDOR (IMPLEMENTATION), RENDERER, GLSL, ETC.:
        std::cout << std::setw(34) << std::left << "OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
        std::cout << std::setw(34) << std::left << "OpenGL Shading Language Version: " << (char *)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << std::setw(34) << std::left << "OpenGL Vendor:" << (char *)glGetString(GL_VENDOR) << std::endl;
        std::cout << std::setw(34) << std::left << "OpenGL Renderer:" << (char *)glGetString(GL_RENDERER) << std::endl;
        #ifndef NDEBUG
        std::cout<<"Window::init_SDL END"<<std::endl;
        #endif
    }
    void Window::destroy_window()
    {
        if (handle_)
        {
            SDL_DestroyWindow(handle_);
        }
        SDL_Quit();
        handle_ = nullptr;
    }
    void Window::wait_events()
    {
        SDL_WaitEvent(&event_);
    }
    void Window::set_factor()
    {
        int frame_width, frame_height;
        int window_width, window_height;
        SDL_GL_GetDrawableSize(handle_, &frame_width, &frame_height);
        SDL_GetWindowSize(handle_, &window_width, &window_height);
        if (window_width != 0)
            factor_ = frame_width / window_width;
        else
            factor_ = 1;
    }

    void Window::set_size(uint32_t width, uint32_t height)
    {
        width_ = width;
        height_ = height;
    }

    SDL_Window *Window::get_handle() const
    {
        return handle_;
    }

    SDL_GLContext &Window::get_context(){
        return gl_context_;
    }
    
    SDL_Event &Window::get_event(){
        return event_;
    }

    std::pair<uint32_t, uint32_t> Window::get_size() const
    {
        return std::make_pair(width_, height_);
    }

    uint32_t Window::get_width() const
    {
        return width_ * factor_;
    }
    uint32_t Window::get_height() const
    {
        return height_ * factor_;
    }
    std::string Window::get_title() const
    {
        return title_;
    }
    float Window::get_factor()
    {
        set_factor();
        return factor_;
    }
    float Window::get_aspect() const
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    std::pair<int, int> Window::get_framebuffer_size() const
    {
        std::pair<int, int> size{0, 0};
        SDL_GL_GetDrawableSize(handle_, &size.first, &size.second);
        return size;
    }
    void Window::update_window()
    {
        int width;
        int height;
        SDL_GetWindowSize(handle_, &width, &height);
        width_ = width;
        height_ = height;
        set_factor();
    }

    void Window::swap_window(){
        SDL_GL_SwapWindow(handle_);
    }

}