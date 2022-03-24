
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void draw_imgui();

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
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int pixelate_factor = 100;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glcpp::Window g_window(SCR_WIDTH, SCR_HEIGHT, "glcpp-test");

int main()
{
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(g_window.get_handle(), framebuffer_size_callback);
    glfwSetCursorPosCallback(g_window.get_handle(), mouse_callback);
    glfwSetScrollCallback(g_window.get_handle(), scroll_callback);
    // tell GLFW to capture our mouse
    // glfwSetInputMode(g_window.get_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // build and compile shaders
    // -------------------------
    Shader ourShader("./../../resources/shaders/1.model_loading.vs", "./../../resources/shaders/1.model_loading.fs");

    // load models
    // -----------
    Model ourModel("./../../resources/models/res/model.dae");
    std::vector<std::string> skybox_faces{"./../../resources/textures/skybox/right.jpg",
                                          "./../../resources/textures/skybox/left.jpg",
                                          "./../../resources/textures/skybox/top.jpg",
                                          "./../../resources/textures/skybox/bottom.jpg",
                                          "./../../resources/textures/skybox/front.jpg",
                                          "./../../resources/textures/skybox/back.jpg"};
    glcpp::Cubemap skybox(skybox_faces,
                          "./../../resources/shaders/skybox.vs",
                          "./../../resources/shaders/skybox.fs");

    glcpp::Framebuffer framebuffer(SCR_WIDTH, SCR_HEIGHT, "./../../resources/shaders/simple_framebuffer.vs", "./../../resources/shaders/simple_framebuffer.fs");

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
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

        // capture model
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.get_fbo());

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // don't forget to enable shader before setting uniforms

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), g_window.get_aspect(), 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

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
        ourModel.Draw(ourShader);

        // end capture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.draw(camera, projection);

        framebuffer.set_pixelate_factor(pixelate_factor);

        framebuffer.draw();

        // framebuffer.print_color_texture("file.png");

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

bool isOkay = false;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        isOkay = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        isOkay = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    g_window.update_window();
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (isOkay)
    {
        return;
    }
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void draw_imgui()
{
    // render your GUI
    ImGui::Begin("animation");
    ImGui::Button("RUN");
    ImGui::Button("STOP");
    ImGui::SliderFloat("model rotate z", &model_rotation_z, 0.0f, 6.5f);
    ImGui::SliderFloat("model rotate x", &model_rotation_x, 0.0f, 6.5f);
    ImGui::SliderFloat("model rotate y", &model_rotation_y, 0.0f, 6.5f);
    ImGui::SliderFloat("model size", &model_size, 0.0f, 5.0f);
    ImGui::SliderInt("size", &pixelate_factor, 32, SCR_WIDTH * 2);
    ImGui::End();
    // render your GUI
    ImGui::Begin("palette");
    ImGui::Button(std::to_string(g_window.get_aspect()).c_str());
    ImGui::End();
    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}