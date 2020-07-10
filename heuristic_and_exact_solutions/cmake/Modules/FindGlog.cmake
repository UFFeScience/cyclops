# FindGlog.cmake exports:
#
# GLOG_FOUND: TRUE iff glog is found.
# GLOG_LIBRARIES: Libraries required to link glog.

list(APPEND GLOG_CHECK_INCLUDE_DIRS
     /usr/local/include
     /opt/local/include
     /usr/include)
find_path(GLOG_INCLUDE_DIR
          NAMES glog/logging.h
          HINTS ${GLOG_INCLUDE_DIR_HINTS}
          PATHS ${GLOG_CHECK_INCLUDE_DIRS}
          PATH_SUFFIXES ${GLOG_CHECK_PATH_SUFFIXES})
if(GLOG_INCLUDE_DIR)
  set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
endif()

list(APPEND GLOG_CHECK_LIBRARY_DIRS
     /usr/local/lib
     /opt/local/lib
     /usr/lib)
find_library(GLOG_LIBRARY NAMES glog
             HINTS ${GLOG_LIBRARY_DIR_HINTS}
             PATHS ${GLOG_CHECK_LIBRARY_DIRS}
             PATH_SUFFIXES ${GLOG_CHECK_LIBRARY_SUFFIXES})
if(GLOG_LIBRARY)
  set(GLOG_LIBRARIES ${GLOG_LIBRARY})
  set(GLOG_FOUND TRUE)
endif()
