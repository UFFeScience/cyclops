file(GLOB docs  ${CMAKE_SOURCE_DIR}/docs/html/*
                ${CMAKE_SOURCE_DIR}/docs/stats/*
                ${CMAKE_SOURCE_DIR}/docs/coverage/*)
list(REMOVE_ITEM docs "${CMAKE_SOURCE_DIR}/docs/html/.gitignore")
list(REMOVE_ITEM docs "${CMAKE_SOURCE_DIR}/docs/stats/.gitignore")
list(REMOVE_ITEM docs "${CMAKE_SOURCE_DIR}/docs/coverage/.gitignore")

foreach(file ${docs})
  if (EXISTS ${file})
     file(REMOVE_RECURSE ${file})
  endif()
endforeach(file)
