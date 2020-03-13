cmake_minimum_required(VERSION 2.6)
project(wf_security_approximate_solutions)
# The version number.
set(WF_SECURITY_VERSION_MAJOR 0)
set(WF_SECURITY_VERSION_MINOR 1)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default C++ flags
set(COMMON "-fdiagnostics-color")
set(WARNINGS "-Wall -Wextra -Wshadow -Wsign-conversion -Wsign-promo -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON} ${WARNINGS}")

# output directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# additional cmake options
include(CMakeToolsHelpers OPTIONAL)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# additional include directories (-I in g++).
include_directories(./)

##### Dependencies

find_package(Glog REQUIRED)
find_package(gflags REQUIRED)

set(gflags_DIR /usr/lib64/cmake/gflags)

##### sources

file(GLOB         MAIN    "src/main.*")
file(GLOB_RECURSE HEADERS "src/*/*.h")
file(GLOB_RECURSE SOURCES "src/*/*.cc")

##### executables
# include_directories(SYSTEM ${Gflags_INCLUDE_DIR})
add_executable(wf_security_greedy.x ${MAIN} ${HEADERS} ${SOURCES})
target_link_libraries(wf_security_greedy.x glog::glog gflags)
