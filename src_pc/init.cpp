#include <init.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

namespace {
GLFWwindow *window;
std::vector<std::string> args;
}

void init(int argc, const char** argv) {
    for(int i = 0; i < argc; i++)
        args.push_back(argv[i]);
    if(!glfwInit())
        throw "GLFW could not be initialized";
    glfwSetErrorCallback([](int error, const char* description) {
                    std::cerr << "Error " <<  description << "(" << error << ")" << std::endl;
                });
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(640, 480, "Example Game", nullptr, nullptr);
    if(!window)
        throw "Window could not be created";
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwSwapInterval(1);
}
bool processEvents() {
    glfwPollEvents();
    return !glfwWindowShouldClose(window);
}
void swapFB() {
    glfwSwapBuffers(window);
}
void deinit() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

std::vector<std::string> &getArgs() {
    return args;
}
