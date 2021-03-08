set_source_files_properties(src/glad.c PROPERTIES LANGUAGE CXX)

set(suffix core-4.6)

add_library(glad-${suffix} src/glad.c)

target_include_directories(glad-${suffix} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

if(TARGET OpenGL::OpenGL)
  target_link_libraries(glad-${suffix} PUBLIC OpenGL::OpenGL)
else()
  target_link_libraries(glad-${suffix} PUBLIC OpenGL::GL)
endif()
if(TARGET OpenGL::GLU)
  target_link_libraries(glad-${suffix} PUBLIC OpenGL::GLU)
endif()
target_link_libraries(glad-${suffix} PUBLIC ${CMAKE_DL_LIBS})

set_target_properties(glad-${suffix} PROPERTIES
  POSITION_INDEPENDENT_CODE ON)
