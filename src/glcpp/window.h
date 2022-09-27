#ifndef GLCPP_WINDOW_WINDOW_H
#define GLCPP_WINDOW_WINDOW_H

#include <glad/glad.h>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace glcpp
{
    class Window
    {

    public:
        Window() = delete;
        Window(uint32_t width, uint32_t height, std::string title);
        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;
        virtual ~Window();

        bool should_close();
        void process_events();
        void init_glfw();
        void destroy_window();
        void wait_events();

        void set_factor();
        void set_size(uint32_t width, uint32_t height);

        SDL_Window *get_handle() const;
        SDL_GLContext &get_context();
        SDL_Event& get_event();
        std::pair<uint32_t, uint32_t> get_size() const;
        uint32_t get_width() const;
        uint32_t get_height() const;
        std::string get_title() const;
        std::pair<int, int> get_framebuffer_size() const;
        float get_factor();
        float get_aspect() const;
        void update_window();
        void swap_window();

    private:
        SDL_Window *handle_ = nullptr;
        SDL_GLContext gl_context_ = nullptr;
        SDL_Event event_ = { 0 };
        bool is_window_close_ = false;

        float factor_;
        uint32_t width_ = 512;
        uint32_t height_ = 512;
        std::string title_ = "";
    };
} // namespace vkcpp
#endif // #ifndef VKCPP_MAIN_WINDOW_H