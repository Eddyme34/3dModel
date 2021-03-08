# build for opengl2
add_library(imgui-compatible 
  imgui.cpp
  imgui_draw.cpp
  imgui_demo.cpp
  imgui_widgets.cpp
  examples/imgui_impl_glfw.cpp
  examples/imgui_impl_opengl2.cpp)
target_link_libraries(imgui-compatible glad-compatible ${gl_libs})
target_include_directories(imgui-compatible PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/examples/opengl2_example>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/examples>)
target_compile_definitions(imgui-compatible PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# build for opengl3
foreach(suffix ${ALL_GL_VERSIONS})

  add_library(imgui-${suffix}
    imgui.cpp
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_widgets.cpp
    examples/imgui_impl_glfw.cpp
    examples/imgui_impl_opengl3.cpp)
  target_link_libraries(imgui-${suffix} glad-${suffix} ${gl_libs})
  target_include_directories(imgui-${suffix} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/examples/opengl3_example>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/examples>)
  target_compile_definitions(imgui-${suffix} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
  
endforeach()
