add_executable(run_p3
  main.cpp
  geometry_triangle.cpp)
target_link_libraries(run_p3 PUBLIC util-core-3.3 glm)
set_target_properties(run_p3 PROPERTIES CXX_STANDARD 11) 
target_add_embeded_shaders(run_p3
  OUTPUT_NAME shaders.h SHADERS 
  vshader=shading_vertex.glsl 
  fshader=shading_fragment.glsl
  vshader_quad=shader_passthrough.glsl
  fshader_quad=shader_wobbly_texture.glsl)
