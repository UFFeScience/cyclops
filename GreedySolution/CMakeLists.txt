cmake_minimum_required(VERSION 2.6)
project(wf_security)
# The version number.
set(WF_SECURITY_VERSION_MAJOR 0)
set(WF_SECURITY_VERSION_MINOR 1)

##### Dependencies

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

##### sources

file(GLOB         MAIN    "src/main.*")
file(GLOB_RECURSE HEADERS "src/*/*.h")
file(GLOB_RECURSE SOURCES "src/*/*.cc")

# configure a header file to pass some of the CMake settings
# to the source code
# configure_file (
  # "${PROJECT_SOURCE_DIR}/*.h"
  # "${PROJECT_BINARY_DIR}/*.h"
# )

##### executables

add_executable(wf_security_greedy.x)
