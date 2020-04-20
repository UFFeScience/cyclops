cmake_minimum_required(VERSION 2.6)
project(wf_security_approximate_solutions)
# The version number.
set(WF_SECURITY_VERSION_MAJOR 0)
set(WF_SECURITY_VERSION_MINOR 1)

# auxiliary cmake files should be placed in /cmake/Modules/ .
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default C++ flags
set(COMMON "-fdiagnostics-color -g")
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

##### auxiliary make directives
# cpplint
add_custom_target(cpplint
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND cpplint --recursive --quiet --linelength=100 --extensions=h,cc --filter=-runtime/references,-whitespace/empty_loop_body src/ test/
  USES_TERMINAL
)

# flake8
add_custom_target(flake8
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND flake8 --max-line-length=100 util/ api/
  USES_TERMINAL
)

# docs
add_custom_target(docs
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND doxygen
  COMMAND gitstats . docs/stats
  USES_TERMINAL
)

# coverage
set(COVERAGE_INFO_FILE "docs/coverage/coverage.info")
add_custom_target(coverage
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  DEPENDS nanoEORsimulator-test.x
  COMMAND nanoEORsimulator-test.x &>/dev/null
  COMMAND lcov --quiet --no-external --capture --directory . --output-file ${COVERAGE_INFO_FILE}
  COMMAND lcov --quiet --remove ${COVERAGE_INFO_FILE} 'test/*' -o ${COVERAGE_INFO_FILE}
  COMMAND genhtml ${COVERAGE_INFO_FILE} --quiet --show-details --legend --output-directory docs/coverage/
  COMMAND lcov --list ${COVERAGE_INFO_FILE}
  USES_TERMINAL
)

# clean-docs
add_custom_target(clean-docs
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/clean-docs.cmake
)

# clean-results
add_custom_target(clean-results
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/clean-results.cmake
)

# clean-debug
add_custom_target(clean-debug
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/clean-debug.cmake
)

# clean-cmake
add_custom_target(clean-cmake
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/clean-cmake.cmake
)

# clean-all
add_custom_target(clean-all
  COMMAND ${CMAKE_BUILD_TOOL} clean
  COMMAND ${CMAKE_BUILD_TOOL} clean-docs
  COMMAND ${CMAKE_BUILD_TOOL} clean-results
  COMMAND ${CMAKE_BUILD_TOOL} clean-debug
  COMMAND ${CMAKE_BUILD_TOOL} clean-cmake
)
