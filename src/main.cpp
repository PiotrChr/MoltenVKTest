#include <iostream>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_MACOS_MVK
#include <filesystem>
#include <cstdlib>

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(_WIN32)
#include <Windows.h>
#endif
#include "app.cpp"

std::filesystem::path getExecutablePath() {
    std::vector<char> buffer;
    uint32_t size;

#if defined(__APPLE__)
    _NSGetExecutablePath(nullptr, &size);
    buffer.resize(size);
    _NSGetExecutablePath(buffer.data(), &size);
#elif defined(_WIN32)
    buffer.resize(MAX_PATH);
    size = GetModuleFileNameA(nullptr, buffer.data(), buffer.size());
    buffer.resize(size);
#else
    // Fallback for other platforms
    buffer.resize(PATH_MAX);
    ssize_t result = readlink("/proc/self/exe", buffer.data(), buffer.size());
    buffer.resize(result);
#endif

    return std::filesystem::path(buffer.data());
}

int main(int argc, const char* argv[]) {
    App app{};
    
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}

