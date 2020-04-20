# FindSUNDIALS.cmake exports:
#
#	SUNDIALS_LIBRARIES
#	SUNDIALS_FOUND
#


find_library(SUNDIALS_LIB_sundials_ida
NAMES sundials_ida
HINTS $ENV{SUNDIALS}/lib)
if(SUNDIALS_LIB_sundials_ida)
  set(SUNDIALS_LIBRARIES ${SUNDIALS_LIBRARIES} ${SUNDIALS_LIB_sundials_ida})
endif()

find_library(SUNDIALS_LIB_sundials_nvecserial
NAMES sundials_nvecserial
HINTS $ENV{SUNDIALS}/lib)
if(SUNDIALS_LIB_sundials_nvecserial)
  set(SUNDIALS_LIBRARIES ${SUNDIALS_LIBRARIES} ${SUNDIALS_LIB_sundials_nvecserial})
endif()

if(SUNDIALS_LIBRARIES)
  set(SUNDIALS_LIBRARIES ${SUNDIALS_LIBRARIES})
  set(SUNDIALS_FOUND TRUE)
endif()

find_package_handle_standard_args(SUNDIALS DEFAULT_MSG SUNDIALS_LIBRARIES)
