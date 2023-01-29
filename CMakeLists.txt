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
find_package(RapidJSON REQUIRED)
find_package(Boost COMPONENTS context fiber date_time url REQUIRED)
find_package(libasyik REQUIRED)

include_directories(
    include
)

add_library(common_utils
    src/base64.cpp
    src/error.cpp
)

add_library(image_resizer
    src/image_resizer.cpp
)

add_executable(${PROJECT_NAME}
    src/image_resizer_app.cpp
)

target_link_libraries(${PROJECT_NAME} common_utils image_resizer)

if(OpenCV_FOUND)
    target_include_directories(image_resizer PUBLIC ${OpenCV_INCLUDE_DIR})
    target_link_libraries(image_resizer ${OpenCV_LIBS})
endif()

if(RapidJSON_FOUND)
    target_include_directories(image_resizer PUBLIC ${RapidJSON_INCLUDE_DIRS})
endif()

if(Boost_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} Boost::fiber Boost::context Boost::date_time Boost::url)
endif()

if(libasyik_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC ${libasyik_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} libasyik)
endif()

if(RUN_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()