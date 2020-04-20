# FindTCLAP.cmake exports:
#
#	TCLAP_FOUND
#	TCLAP_INCLUDE_DIR
#
include(FindPackageHandleStandardArgs)

find_path(TCLAP_INCLUDE_DIR
        NAMES tclap/CmdLine.h
        PATHS ${TCLAP_PKGCONF_INCLUDE_DIRS}
        )

if(TCLAP_PKGCONF_FOUND)
    set(TCLAP_INCLUDE_DIR ${TCLAP_INCLUDE_DIR} ${TCLAP_PKGCONF_INCLUDE_DIRS})
    set(TCLAP_FOUND yes)
endif(TCLAP_PKGCONF_FOUND)

find_package_handle_standard_args(TCLAP DEFAULT_MSG TCLAP_INCLUDE_DIR)
