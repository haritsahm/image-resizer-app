include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(image_resizer_test test-basic-base64.cpp)

target_link_libraries(image_resizer_test
    PRIVATE
    GTest::GTest
    base64)

add_test(image_resizer_gtests image_resizer_test)
