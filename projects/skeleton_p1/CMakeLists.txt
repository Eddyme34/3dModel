add_executable(run_p1 main.cpp comm.cpp)
target_link_libraries(run_p1 PUBLIC util-compatible ${gl_libs})
set_target_properties(run_p1 PROPERTIES CXX_STANDARD 11) 
