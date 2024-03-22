cmake_minimum_required(VERSION 3.13)    # CMake version check
project(wf_security)                    # Create project
set(CMAKE_CXX_STANDARD 17)              # Enable c++17 standard

# Auxiliary cmake files should be placed in /cmake/Modules/
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON "-fdiagnostics-color -g")
set(WARNINGS "-Wall -Wextra -Wshadow -Wsign-conversion -Wsign-promo -Wpedantic")

set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} ${COMMON} ${WARNINGS}")
set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -Og -ggdb -Wno-unknown-pragmas")
set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3 -flto -Wno-unknown-pragmas")
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX  "-isystem ")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(LINKER_OPTIONS                  -flto -Wl,--no-as-needed)

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    set(CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG} -save-temps=obj")
endif()

find_package(Glog   REQUIRED)
find_package(gflags REQUIRED)
find_package(Boost  REQUIRED)
find_package(CPLEX  REQUIRED)
include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS})

# additional cmake options
include(CMakeToolsHelpers OPTIONAL)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

# additional include directories (-I in g++).
include_directories(./)

##### Dependencies

#add_definitions(-DNDEBUG)
add_definitions(-DIL_STD)
set(CPLEX_PREFIX_DIR      /opt/ibm/ILOG/CPLEX_Studio221)
set(CPLEX_INCLUDE_DIR     ${CPLEX_PREFIX_DIR}/cplex/include)
set(CPLEX_LIBRARIES_DIR   ${CPLEX_PREFIX_DIR}/cplex/lib/x86-64_linux/static_pic)
set(CONCERT_INCLUDE_DIR   ${CPLEX_PREFIX_DIR}/concert/include)
set(CONCERT_LIBRARIES_DIR ${CPLEX_PREFIX_DIR}/concert/lib/x86-64_linux/static_pic)
include_directories(${CPLEX_INCLUDE_DIR} ${CONCERT_INCLUDE_DIR})
link_directories(${CPLEX_LIBRARIES_DIR} ${CONCERT_LIBRARIES_DIR})

set(gflags_DIR /usr/lib64/cmake/gflags)


##### sources

file(GLOB         MAIN    "src/main.*")
file(GLOB_RECURSE HEADERS "src/*/*.h")
file(GLOB_RECURSE SOURCES "src/*/*.cc")

##### executables
add_executable(wf_security_greedy.x ${MAIN} ${HEADERS} ${SOURCES})
target_link_libraries(wf_security_greedy.x ${CPLEX_LIBRARIES} ${GLOG_LIBRARIES} gflags dl)

##### auxiliary make directives
add_custom_target(cpplint
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND cpplint --recursive --quiet --linelength=100 --extensions=h,cc --filter=-runtime/references,-whitespace/empty_loop_body src/ test/
        USES_TERMINAL
        )

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
