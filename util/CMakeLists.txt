# here we are building one target for each OpenGL version
foreach(suffix compatible ${ALL_GL_VERSIONS})

  # create the library
  add_library(util-${suffix} util.cpp arcball_camera.cpp)

  # add library dependencies
  target_link_libraries(util-${suffix} PUBLIC
    glad-${suffix} 
    imgui-${suffix} 
    glm 
    ${gl_libs})

  # setup include directries for this target (modern cmake)
  target_include_directories(util-${suffix} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

  # and other properties
  set_target_properties(util-${suffix} PROPERTIES CXX_STANDARD 11)

endforeach()
