set(debug ${CMAKE_SOURCE_DIR}/bin/nanoEORsimulator.x.dSYM
          ${CMAKE_SOURCE_DIR}/bin/nanoEORsimulator-test.x.dSYM
          ${CMAKE_SOURCE_DIR}/bin/nanoEORsimulator-stress-test.x.dSYM
)

foreach(folder ${debug})
  if (EXISTS ${folder})
     file(REMOVE_RECURSE ${folder})
  endif()
endforeach(folder)
