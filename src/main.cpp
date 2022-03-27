
//개발동기- 단점,
// 요구사항- 분석 - 설계 /////- 구현 - 개발 - 유지보수
//       설계툴(카카오오븐 등)
// 서론 / 개발동기/ 문제점/ 관련연구(국내 , 국외)ㅏ/ 문제점, 장점, 단점
// 본론/ 전체 시스템 구성도
// 결론/ 기여도 (사회 기여도)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glcpp/shader.h>
#include <glcpp/camera.h>
#include <glcpp/model.h>
#include <glcpp/window.h>
#include <glcpp/cubemap.h>
#include <glcpp/framebuffer.h>

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <iostream>
#include <vector>
#include <nfd.h>
#include <memory>

#include <filesystem>

namespace fs = std::filesystem;

void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void draw_imgui();
void draw_model(glcpp::Model &ourModel, Shader &ourShader, const glm::mat4 &view, const glm::mat4 &projection);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// model attribute
glm::vec3 model_translation(0.0f, 0.0f, 0.0f);
float model_size = 1.0f;
float model_rotation_z = 0.0f;
float model_rotation_x = 0.0f;
float model_rotation_y = 0.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
bool is_pressed = false;
bool is_pressed_scroll = false;
glm::vec2 prev_mouse{-1.0f, -1.0f}, cur_mouse{-1.0f, -1.0f};

int pixelate_factor = 1000;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glcpp::Window g_window(SCR_WIDTH, SCR_HEIGHT, "glcpp-test");
glcpp::Model ourModel(fs::canonical(fs::path("./../../resources/models/nanosuit/nanosuit.obj")).string().c_str());
std::unique_ptr<glcpp::Framebuffer> model_framebuffer;
std::unique_ptr<glcpp::Framebuffer> skybox_framebuffer;
std::unique_ptr<glcpp::Framebuffer> RGB_fb;
std::unique_ptr<glcpp::Framebuffer> RGBA_fb;
std::unique_ptr<glcpp::Framebuffer> target_fb;

// skybox
std::vector<std::string> skybox_faces[]{
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
     "./../../resources/textures/cube/pisa/nz.png"}};

bool is_skybox_blur = false;

int main()
{
    NFD_Init();
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    glfwSetFramebufferSizeCallback(g_window.get_handle(), framebuffer_size_callback);
    glfwSetCursorPosCallback(g_window.get_handle(), mouse_callback);
    glfwSetScrollCallback(g_window.get_handle(), scroll_callback);
    glfwSetMouseButtonCallback(g_window.get_handle(), mouse_btn_callback);
    // tell GLFW to capture our mouse
    //  glfwSetInputMode(g_window.get_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // build and compile shaders
    // -------------------------
    Shader ourShader("./../../resources/shaders/1.model_loading.vs", "./../../resources/shaders/1.model_loading.fs");
    Shader pixelateShader("./../../resources/shaders/simple_framebuffer.vs", "./../../resources/shaders/pixelate_framebuffer.fs");
    Shader frameShader("./../../resources/shaders/simple_framebuffer.vs", "./../../resources/shaders/simple_framebuffer.fs");
    Shader debugShader("./../../resources/shaders/1.model_loading.vs", "./../../resources/shaders/debug_model.fs");

    // load models
    // -----------
    glcpp::Cubemap skybox(skybox_faces[0],
                          "./../../resources/shaders/skybox.vs",
                          "./../../resources/shaders/skybox.fs");

    g_window.set_factor();

    model_framebuffer = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height(), GL_RGB);
    skybox_framebuffer = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height());
    RGB_fb = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height(), GL_RGB);
    RGBA_fb = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height(), GL_RGBA);
    target_fb = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height(), GL_RGBA);
    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(g_window.get_handle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    //  Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // render loop
    // -----------
    while (!g_window.should_close())
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;

        lastFrame = currentFrame;

        // input
        // -----
        processInput(g_window.get_handle());
        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), g_window.get_aspect(), 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glBindFramebuffer(GL_FRAMEBUFFER, RGB_fb->get_fbo());
        {
            glEnable(GL_DEPTH_TEST);
            // model capture
            glViewport(0, 0, RGB_fb->get_width(), RGB_fb->get_height());
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw_model(ourModel, ourShader, view, projection);
            RGB_fb->print_color_texture("RGB.png");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, RGBA_fb->get_fbo());
        {
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_DEPTH_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            // model capture
            glViewport(0, 0, RGBA_fb->get_width(), RGBA_fb->get_height());
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            draw_model(ourModel, debugShader, view, projection);

            glStencilFunc(GL_EQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            RGB_fb->draw(frameShader);

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glEnable(GL_DEPTH_TEST);
            RGBA_fb->print_color_texture("RGBA.png");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glViewport(0, 0, g_window.get_width(), g_window.get_height());
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            skybox.draw(view, projection);
            RGBA_fb->draw(frameShader);
            glDisable(GL_BLEND);
        }

        draw_imgui();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(g_window.get_handle());
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    g_window.update_window();

    model_framebuffer.reset();
    skybox_framebuffer.reset();
    model_framebuffer = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height());
    skybox_framebuffer = std::make_unique<glcpp::Framebuffer>(g_window.get_width(), g_window.get_height());
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (is_pressed)
    {
        camera.ProcessMouseMovement((static_cast<float>(yposIn) - prev_mouse.y) / 3.6f, (static_cast<float>(xposIn) - prev_mouse.x) / 3.6f);
        prev_mouse.x = xposIn;
        prev_mouse.y = yposIn;
    }
    cur_mouse.x = xposIn;
    cur_mouse.y = yposIn;
}
void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse)
    {
        prev_mouse.x = cur_mouse.x;
        prev_mouse.y = cur_mouse.y;
        is_pressed = true;
    }
    else
    {
        is_pressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        is_pressed_scroll = true;
    }
    else
    {
        is_pressed_scroll = false;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
    if (is_pressed_scroll)
    {
        // TODO: MOVE TO CAMERA
    }
}

void draw_imgui()
{
    // render your GUI
    ImGui::Begin("model");
    {
        // open Dialog Simple
        if (ImGui::Button("OPEN"))
        {
            nfdchar_t *outPath;
            nfdfilteritem_t filterItem[2] = {{"model file", "obj,dae,pmx"}, {"error format", "gltf, dae"}};
            nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);

            if (result == NFD_OKAY)
            {
                puts("Success!");
                puts(outPath);
                ourModel = glcpp::Model(outPath);
                NFD_FreePath(outPath);
            }
            else if (result == NFD_CANCEL)
            {
                puts("User pressed cancel.");
            }
            else
            {
                printf("Error: %s\n", NFD_GetError());
            }

            NFD_Quit();
        }
        if (ImGui::Button("BlurSkybox"))
        {
            is_skybox_blur = !is_skybox_blur;
        }

        ImGui::SliderFloat("model rotate z", &model_rotation_z, 0.0f, 6.5f);
        ImGui::SliderFloat("model rotate x", &model_rotation_x, 0.0f, 6.5f);
        ImGui::SliderFloat("model rotate y", &model_rotation_y, 0.0f, 6.5f);
        ImGui::SliderFloat("model size", &model_size, 0.0f, 5.0f);
        ImGui::SliderInt("size", &pixelate_factor, 32, SCR_WIDTH * 2);
    }
    ImGui::End();
    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void draw_model(glcpp::Model &ourModel, Shader &ourShader, const glm::mat4 &view, const glm::mat4 &projection)
{
    ourShader.use();

    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, model_translation);                         // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(model_size, model_size, model_size)); // it's a bit too big for our scene, so scale it down
    model = glm::rotate(model, model_rotation_z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, model_rotation_x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, model_rotation_y, glm::vec3(0.0f, 1.0f, 1.0f));
    ourShader.setMat4("model", model);
    ourModel.draw(ourShader);
}