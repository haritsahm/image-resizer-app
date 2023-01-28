cmake_minimum_required(VERSION 3.15)
project(image_resizer_app)

set(CMAKE_CXX_STANDARD 14)
option(RUN_TESTS "Wether to run tests" OFF)

FIND_PROGRAM(GCOV_PATH gcov)
FIND_PROGRAM(LCOV_PATH lcov)
FIND_PROGRAM(GENHTML_PATH genhtml)

if(RUN_TESTS)
    message("Building with lcov Code Coverage Tools")

    # Warning/Error messages
    if(NOT(CMAKE_BUILD_TYPE STREQUAL "Debug"))
        message(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
    endif()

    if(NOT LCOV_PATH)
        message(FATAL_ERROR "lcov not found! Aborting...")
    endif()

    if(NOT GENHTML_PATH)
        message(FATAL_ERROR "genhtml not found! Aborting...")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
endif()

find_package(OpenCV 4 REQUIRED)

include_directories(
    include
)

add_library(base64
    src/base64.cpp
)

if(RUN_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()