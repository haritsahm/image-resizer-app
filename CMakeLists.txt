cmake_minimum_required(VERSION 3.15)
project(image_resizer_app)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    include
)

add_library(base64
    src/base64.cpp
)

