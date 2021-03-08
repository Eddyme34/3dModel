function(target_add_embeded_shaders target)
  # how to parse keyword arguments in a cmake function
  set(options)
  set(oneArgs OUTPUT_NAME)
  set(mulArgs SHADERS)
  cmake_parse_arguments(in "${options}" "${oneArgs}" "${mulArgs}" ${ARGN})
  if(in_UNPARSED_ARGUMENTS)
    foreach(arg ${in_UNPARSED_ARGUMENTS})
      message(WARNING "Unparsed argument: ${arg}")
    endforeach()
  endif()
  # compute a command for bin2c
  unset(all_shaders)
  set(the_command bin2c -o ${in_OUTPUT_NAME})
  foreach(s ${in_SHADERS})
    # the shader is provided in the form of 'name=filename'
    if("${s}" MATCHES "^([^=]*)=([^=]*)$")
      list(APPEND all_shaders ${CMAKE_MATCH_2})
      set(the_command ${the_command} -n ${CMAKE_MATCH_1} ${CMAKE_MATCH_2})
    endif()
    # the shader is provided in the form of 'filename'
    if("${s}" MATCHES "^([^=]*)$")
      list(APPEND all_shaders ${CMAKE_MATCH_1})
      set(the_command ${the_command} ${CMAKE_MATCH_1})
    endif()
  endforeach()
  # now generate the file
  string(REPLACE "/" "_" output_identifier ${in_OUTPUT_NAME})
  add_custom_target(_embeded_shaders_${target}_${output_identifier}
    COMMAND ${the_command}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "generate embeded shaders for ${target}"
    SOURCES ${all_shaders})
  add_dependencies(${target} _embeded_shaders_${target}_${output_identifier})
endfunction()
