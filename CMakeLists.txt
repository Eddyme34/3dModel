# welcome to ECS 175
cmake_minimum_required(VERSION 3.0)
project(ecs175-template)

# enable rpath on macos (advanced configuration)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_MACOSX_RPATH 1)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# --------------------------------------
# load/build required libraries
#
# ---> OpenGL (advanced configuration)
# reference https://cmake.org/cmake/help/v3.18/module/FindOpenGL.html
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
# correctly select OpenGL targets (modern CMake)
if(TARGET OpenGL::OpenGL)
  list(APPEND gl_libs OpenGL::OpenGL)
else()
  list(APPEND gl_libs OpenGL::GL)
endif()
if(TARGET OpenGL::GLU)
  list(APPEND gl_libs OpenGL::GLU)
endif()

# ---> build glfw from source
# use GLFW for interactive GL rendering 
# as it is cross platform compatable
if(TARGET OpenGL::GLX)
  list(APPEND gl_libs OpenGL::GLX)
endif()
set(GLFW_USE_OSMESA OFF)
# configure other options
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${PROJECT_SOURCE_DIR}/glfw-3.3.2)
# hide these options by default (try the GUI interface :D)
mark_as_advanced(GLFW_INSTALL 
  GLFW_BUILD_DOCS GLFW_BUILD_TESTS GLFW_BUILD_EXAMPLES
  GLFW_USE_OSMESA GLFW_USE_WAYLAND GLFW_VULKAN_STATIC)
list(APPEND gl_libs glfw)

# ---> create glm target
add_subdirectory(glm-0.9.9.8)

# ---> build glad from source
add_subdirectory(glad-compatible)
add_subdirectory(glad-core-3.3)
add_subdirectory(glad-core-4.1)
add_subdirectory(glad-core-4.6) # might work only on the latest GPUs
set(ALL_GL_VERSIONS core-3.3 core-4.1 core-4.6)

# ---> use the bin2c tool to embed shader
add_executable(bin2c bin2c/bin2c.c) # compile bin2c obviously
include(bin2c/target_add_embeded_shaders.cmake)

# ---> build "Dear ImGui"
# Note: I updated the version of imgui. If for some reasons your
#       GUI fails to compile due to this change, please comment
#       and uncomment following lines:
# add_subdirectory(imgui-1.75)
add_subdirectory(imgui-1.79)

# ---> build the project utility library
add_subdirectory(util)

# build sub-projects
add_subdirectory(demo)
# add_subdirectory(projects/test_p1)
add_subdirectory(projects/skeleton_p1)
# add_subdirectory(projects/test_p2)
add_subdirectory(projects/skeleton_p2)
# add_subdirectory(projects/test_p3)
add_subdirectory(projects/skeleton_p3)

foreach(f data/cube.obj data/suzanne.obj data/uvmap.jpg)
  file(COPY ${f} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()
