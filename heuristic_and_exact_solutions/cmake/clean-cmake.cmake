set(cmake_generated ${CMAKE_SOURCE_DIR}/CMakeCache.txt
                    ${CMAKE_SOURCE_DIR}/cmake_install.cmake
                    ${CMAKE_SOURCE_DIR}/Makefile
                    ${CMAKE_SOURCE_DIR}/CMakeFiles
                    ${CMAKE_SOURCE_DIR}/CMakeTools
                    ${CMAKE_SOURCE_DIR}/CMakeToolsMeta.txt
                    ${CMAKE_SOURCE_DIR}/CMakeToolsMeta.in.txt
                    ${CMAKE_SOURCE_DIR}/CMakeToolsMeta-Debug.txt
                    ${CMAKE_SOURCE_DIR}/CMakeToolsMeta-Release.txt
                    ${CMAKE_SOURCE_DIR}/CMakeToolsMeta-MinSizeRel.txt
                    ${CMAKE_SOURCE_DIR}/CMakeToolsMeta-RelWithDebInfo.txt
                    ${CMAKE_SOURCE_DIR}/_cmt_tmp.cmake
                    ${CMAKE_SOURCE_DIR}/.ninja_deps
                    ${CMAKE_SOURCE_DIR}/.ninja_log
                    ${CMAKE_SOURCE_DIR}/build.ninja
                    ${CMAKE_SOURCE_DIR}/rules.ninja
                    ${CMAKE_SOURCE_DIR}/compile_commands.json
)

foreach(file ${cmake_generated})
  if (EXISTS ${file})
     file(REMOVE_RECURSE ${file})
  endif()
endforeach(file)
