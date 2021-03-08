# ---> demo 1 (a rotating triangle)
add_executable(d0_hello_triangle d0_hello_triangle/main.cpp)
target_link_libraries(d0_hello_triangle PUBLIC util-compatible)
set_target_properties(d0_hello_triangle PROPERTIES CXX_STANDARD 11)

# ---> demo 2 (famous glxgears)
add_executable(d1_glgears d1_glgears/main.cpp d1_glgears/draw.cpp)
target_link_libraries(d1_glgears PUBLIC util-compatible)
set_target_properties(d1_glgears PROPERTIES CXX_STANDARD 11)

# ---> demo 3 (usage of imgui + opengl2)
add_executable(d2_imgui d2_imgui/main.cpp)
target_link_libraries(d2_imgui PUBLIC util-compatible)
set_target_properties(d2_imgui PROPERTIES CXX_STANDARD 11)

# ---> demo 3 (usage of imgui + opengl3)
add_executable(d3_imgui d3_imgui/main.cpp)
target_link_libraries(d3_imgui PUBLIC util-core-3.3)
set_target_properties(d3_imgui PROPERTIES CXX_STANDARD 11)

# ---> demo 4 (simple shaders + loading from file)
add_executable(d4_simple_shader d4_simple_shader/main.cpp)
target_link_libraries(d4_simple_shader PUBLIC util-core-3.3)
set_target_properties(d4_simple_shader PROPERTIES CXX_STANDARD 11)
message(STATUS "Copying shader to build directory [d4_simple_shader]")
foreach(f d4_simple_shader/shader_d4_vertex.glsl
          d4_simple_shader/shader_d4_fragment.glsl)
  file(COPY ${f} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()

# ---> demo 5 (simple shaders + embeded into C)
add_executable(d5_simple_shader d5_simple_shader/main.cpp)
target_link_libraries(d5_simple_shader PUBLIC util-core-3.3)
set_target_properties(d5_simple_shader PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d5_simple_shader OUTPUT_NAME d5_simple_shader/shaders.h SHADERS
  vertexshader=d5_simple_shader/shader_d5_vertex.glsl
  fragmentshader=d5_simple_shader/shader_d5_fragment.glsl)

# ---> demo 6 (matrices)
add_executable(d6_matrices d6_matrices/main.cpp)
target_link_libraries(d6_matrices PUBLIC util-core-3.3)
set_target_properties(d6_matrices PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d6_matrices OUTPUT_NAME d6_matrices/shaders.h SHADERS
  vertexshader=d6_matrices/shader_d6_transform.glsl
  fragmentshader=d6_matrices/shader_d6_color.glsl)

# ---> demo 7 (drawing a colored cube)
add_executable(d7_colored_cube d7_colored_cube/main.cpp)
target_link_libraries(d7_colored_cube PUBLIC util-core-3.3)
set_target_properties(d7_colored_cube PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d7_colored_cube OUTPUT_NAME d7_colored_cube/shaders.h SHADERS
  vertexshader=d7_colored_cube/shader_d7_vertex.glsl
  fragmentshader=d7_colored_cube/shader_d7_fragment.glsl)

# ---> demo 8 (drawing a textured cube)
add_executable(d8_texture_cube d8_texture_cube/main.cpp)
target_link_libraries(d8_texture_cube PUBLIC util-core-3.3)
set_target_properties(d8_texture_cube PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d8_texture_cube
  OUTPUT_NAME
  d8_texture_cube/shaders.h
  SHADERS
  vertexshader=d8_texture_cube/shader_d8_vertex.glsl
  fragmentshader=d8_texture_cube/shader_v8_fragment.glsl)
foreach(f d8_texture_cube/uvtemplate.bmp)
  file(COPY ${f} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()

# ---> demo 9 (loading OBJ file, rendering using vertex indexing)
add_executable(d9_geometries
  d9_geometries/geometry_triangle.cpp 
  d9_geometries/main.cpp)
target_link_libraries(d9_geometries PUBLIC util-core-3.3)
set_target_properties(d9_geometries PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d9_geometries OUTPUT_NAME d9_geometries/shaders.h SHADERS
  vertexshader=d9_geometries/shader_d9_vertex.glsl
  fragmentshader=d9_geometries/shader_d9_fragment.glsl)

# ---> demo 10 (camera transformation)
add_executable(d10_advanced_camera
  d10_advanced_camera/main.cpp)
target_link_libraries(d10_advanced_camera PUBLIC util-core-3.3)
set_target_properties(d10_advanced_camera PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d10_advanced_camera OUTPUT_NAME d10_advanced_camera/shaders.h SHADERS
  vertexshader=d10_advanced_camera/shader_d10_vertex.glsl
  fragmentshader=d10_advanced_camera/shader_d10_fragment.glsl)

# ---> demo 11 (render to texture)
add_executable(d11_render_to_texture
  d11_render_to_texture/geometry_triangle.cpp 
  d11_render_to_texture/main.cpp)
target_link_libraries(d11_render_to_texture PUBLIC util-core-3.3)
set_target_properties(d11_render_to_texture PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(d11_render_to_texture 
  OUTPUT_NAME d11_render_to_texture/shaders.h SHADERS
  vshader_rtt=d11_render_to_texture/shader_d11_rtt_vertex.glsl
  fshader_rtt=d11_render_to_texture/shader_d11_rtt_fragment.glsl
  vshader_quad=d11_render_to_texture/shader_passthrough.glsl
  fshader_quad=d11_render_to_texture/shader_wobbly_texture.glsl)

# ---> demo 12 (how to order triangles)
add_executable(d12_reordering d12_reordering/main.cpp)
target_link_libraries(d12_reordering PUBLIC util-core-3.3)
set_target_properties(d12_reordering PROPERTIES CXX_STANDARD 11)
target_add_embeded_shaders(
  d12_reordering OUTPUT_NAME d12_reordering/shaders.h SHADERS
  vertexshader=d12_reordering/shader_d12_transform.glsl
  fragmentshader=d12_reordering/shader_d12_color.glsl)