cmake_minimum_required(VERSION 3.0)
project(VulkanTriangle)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # MacOS specific flags and settings
    set(IS_OSX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific flags and settings
    set(IS_LINUX 1)
endif()

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

if (IS_OSX) {
    set(GLFW_LIBRARY "${LIBRARIES_DIR}/libglfw.3.3.dylib")    
}
if (IS_LINUX) {
    set(GLFW_LIBRARY "${LIBRARIES_DIR}/libglfw.so.3.3")
}


find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

# Specify the location of the libraries within the .app bundle

if (IS_OSX) {
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.example.VulkanTriangle
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Info.plist
        RESOURCE "${GLFW_LIBRARY}"
        INSTALL_RPATH "@loader_path/../Resources/lib"
    )
}

if (IS_LINUX) {
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "$ORIGIN/../Resources/lib"
    )

}

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARY} ${Vulkan_LIBRARIES})

# Copy the libraries and validation layers to the Resources/lib folder in the .app bundle
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources"
)