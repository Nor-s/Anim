#include "app.h"
#include <emscripten.h>
// #include <GLFW/glfw3.h>

void mainLoop()
{
	App::get_instance()->loop();
}

int main()
{
	setlocale(LC_ALL, ".utf8");
	App::get_instance()->init(1400, 800, "Anim");
	emscripten_set_main_loop(&mainLoop, 0, 1);
	App::destroy_instance();
	return 0;
}

// #include <emscripten/html5.h>
// #include <cstdio>

// GLFWwindow* window = nullptr;

// static void consoleErrorHandler(int iErrorCode, char const* iErrorMessage)
// {
// 	printf("glfwError: %d | %s\n", iErrorCode, iErrorMessage);
// }

// void mainLoop()
// {
// 	/* Swap front and back buffers */
// 	glfwSwapBuffers(window);
// 	// glfwSwapInterval(0);

// 	/* Poll for and process events */
// 	glfwPollEvents();
// }

// int main()
// {
// 	glfwSetErrorCallback(consoleErrorHandler);

// 	// glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_EMSCRIPTEN);

// 	// printf("GLFW: %s | Platform: 0x%x\n", glfwGetVersionString(), glfwGetPlatform());
// 	// printf("emscripten: v%d.%d.%d\n", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__);

// 	if (!glfwInit())
// 		return -1;

// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
// 	// glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);

// 	int width = 600;
// 	int height = 250;

// 	window = glfwCreateWindow(width, height, "Anim", nullptr, nullptr);
// 	if (!window)
// 	{
// 		glfwTerminate();
// 		return -1;
// 	}

// 	glfwMakeContextCurrent(window);

// 	emscripten_set_main_loop(&mainLoop, 0, 1);
// 	glfwDestroyWindow(window);
// 	glfwTerminate();
// 	return 0;
// }