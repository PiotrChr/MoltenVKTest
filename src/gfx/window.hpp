#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3native.h>
#include <string>
#include <stdexcept>

class VWindow {
    public:
        VWindow(int width, int height, std::string title);
        ~VWindow();

        VWindow(const VWindow&) = delete;
        VWindow& operator=(const VWindow&) = delete;

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        VkExtent2D getExtent() {
            return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        }
        bool shouldClose();
    private:
        int width;
        int height;
        std::string title;
        void initWindow();
        GLFWwindow* window;
};