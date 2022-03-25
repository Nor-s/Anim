#include "window.h"

namespace glcpp
{
    Window::Window(uint32_t width, uint32_t height, std::string title)
        : width_(width), height_(height), title_(title)
    {
        init_window(width_, height_, title_);
    }
    Window::~Window()
    {
        if (handle_)
        {
            destroy_window();
        }
    }
    bool Window::should_close()
    {
        return glfwWindowShouldClose(handle_);
    }
    void Window::close()
    {
        glfwSetWindowShouldClose(handle_, GLFW_TRUE);
    }
    void Window::process_events()
    {
        glfwPollEvents();
    }
    void Window::init_window(uint32_t width, uint32_t height, std::string &title)
    {
        if (!handle_)
        {
            init_glfw();
        }
        else
        {
            set_window(width, height, title);
        }
    }
    void Window::init_glfw()
    {
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW couldn't be initialized.");
        }
        // glfwWindowHint(GLFW_DECORATED, false);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        handle_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
        if (!handle_)
        {
            glfwTerminate();
            throw std::runtime_error("GLFW failed to create window");
        }
        glfwMakeContextCurrent(handle_);
        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }
    void Window::destroy_window()
    {
        glfwDestroyWindow(handle_);
        handle_ = nullptr;
        glfwTerminate();
    }
    void Window::wait_events()
    {
        glfwWaitEvents();
    }
    void Window::set_factor()
    {
        int frame_width, frame_height;
        int window_width, window_height;
        glfwGetFramebufferSize(handle_, &frame_width, &frame_height);
        glfwGetWindowSize(handle_, &window_width, &window_height);
        factor_ = frame_width / window_width;
    }

    void Window::set_window(uint32_t width, uint32_t height, const std::string &title)
    {
        set_size(width, height);
        set_title(title);
    }
    void Window::set_size(uint32_t width, uint32_t height)
    {
        width_ = width;
        height_ = height;
        glfwSetWindowSize(handle_, width_, height_);
    }
    void Window::set_title(const std::string &title)
    {
        title_ = title;
        glfwSetWindowTitle(handle_, title_.c_str());
    }
    void Window::set_user_pointer(void *pointer)
    {
        glfwSetWindowUserPointer(handle_, pointer);
    }
    void Window::set_framebuffer_size_callback(void (*fp)(GLFWwindow *window, int width, int height))
    {
        glfwSetFramebufferSizeCallback(handle_, fp);
    }
    void Window::set_drop_callback(void (*fp)(GLFWwindow *window, int count, const char **paths))
    {
        glfwSetDropCallback(handle_, fp);
    }
    void Window::set_scroll_callback(void (*fp)(GLFWwindow *window, double xoffset, double yoffset))
    {
        glfwSetScrollCallback(handle_, fp);
    }
    void Window::set_mouse_button_callback(void (*fp)(GLFWwindow *window, int button, int action, int modes))
    {
        glfwSetMouseButtonCallback(handle_, fp);
    }
    void Window::set_cursor_pos_callback(void (*fp)(GLFWwindow *window, double xpos, double ypos))
    {
        glfwSetCursorPosCallback(handle_, fp);
    }

    GLFWwindow *Window::get_handle() const
    {
        return handle_;
    }
    std::pair<uint32_t, uint32_t> Window::get_size()
    {
        return std::make_pair(width_, height_);
    }
    uint32_t Window::get_width()
    {
        return width_ * factor_;
    }
    uint32_t Window::get_height()
    {
        return height_ * factor_;
    }
    std::string Window::get_title()
    {
        return title_;
    }
    float Window::get_factor()
    {
        set_factor();
        return factor_;
    }
    float Window::get_aspect()
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }
    std::pair<const char **, uint32_t> Window::get_required_instance_extensions() const
    {
        uint32_t glfw_extension_count;
        auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        return std::make_pair(glfw_extensions, glfw_extension_count);
    }
    std::pair<int, int> Window::get_framebuffer_size()
    {
        std::pair<int, int> size;
        glfwGetFramebufferSize(handle_, &size.first, &size.second);
        // set_size(size.first, size.second);
        return size;
    }
    void Window::update_window()
    {
        int width;
        int height;
        glfwGetWindowSize(handle_, &width, &height);
        width_ = width;
        height_ = height;
        set_factor();
    }
}