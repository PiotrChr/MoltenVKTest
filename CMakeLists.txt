cmake_minimum_required(VERSION 3.0)
project(VulkanTriangle)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# Set the output directory for the .app bundle
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(
    ${PROJECT_NAME}

    src/main.cpp
    src/app.cpp
    src/gfx/window.cpp
    src/gfx/pipeline.cpp
    src/gfx/device.cpp
    src/gfx/swap_chain.cpp
    src/gfx/model.cpp
)

set(LIBRARIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/lib")

set(GLFW_LIBRARY "${LIBRARIES_DIR}/libglfw.3.3.dylib")

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# Specify the location of the libraries within the .app bundle
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_FRAMEWORK_IDENTIFIER com.example.VulkanTriangle
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
    RESOURCE "${GLFW_LIBRARY}"
    INSTALL_RPATH "@loader_path/../Resources/lib"
)

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARY} ${Vulkan_LIBRARIES})

# Copy the libraries and validation layers to the Resources/lib folder in the .app bundle
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources"
COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources"
)