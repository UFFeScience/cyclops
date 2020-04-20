file(GLOB results ${CMAKE_SOURCE_DIR}/results/*
                  ${CMAKE_SOURCE_DIR}/test/results/*
                  ${CMAKE_SOURCE_DIR}/results/snapshots/*
                  ${CMAKE_SOURCE_DIR}/test/results/snapshots/*)
list(REMOVE_ITEM results "${CMAKE_SOURCE_DIR}/results/snapshots")
list(REMOVE_ITEM results "${CMAKE_SOURCE_DIR}/results/.gitignore")
list(REMOVE_ITEM results "${CMAKE_SOURCE_DIR}/test/results/snapshots")
list(REMOVE_ITEM results "${CMAKE_SOURCE_DIR}/test/results/.gitignore")
list(REMOVE_ITEM results "${CMAKE_SOURCE_DIR}/results/snapshots/.gitignore")
list(REMOVE_ITEM results "${CMAKE_SOURCE_DIR}/test/results/snapshots/.gitignore")

foreach(file ${results})
  if (EXISTS ${file})
     file(REMOVE_RECURSE ${file})
  endif()
endforeach(file)
