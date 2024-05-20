cmake_minimum_required(VERSION 3.22)

project(max_flow_rendering)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/BuildFlags.cmake)
include(cmake/TestSolution.cmake)

find_package(Catch REQUIRED)

add_catch(max_flow_rendering Kernel/max_flow.cpp Tests/test_max_flow.cpp
        Library/observer_pattern.h Tests/test_observer_pattern.cpp)
