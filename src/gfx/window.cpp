#include "window.hpp"

VWindow::VWindow(int width, int height, std::string title): width(width), height(height), title(title) {
    initWindow();
}

VWindow::~VWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool VWindow::shouldClose() {
    return glfwWindowShouldClose(window);
}

void VWindow::initWindow() {
    glfwInit();

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    printf("GLFW Version: %d.%d.%d\n", major, minor, rev);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void VWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan surface.");
    }
}
