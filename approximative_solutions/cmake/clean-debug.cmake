set(debug ${CMAKE_SOURCE_DIR}/bin/wf_security_greedy.x.dSYM)

foreach(folder ${debug})
  if (EXISTS ${folder})
     file(REMOVE_RECURSE ${folder})
  endif()
endforeach(folder)
