include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test_basic_base64
    test-basic-base64.cpp
)
target_link_libraries(test_basic_base64
    PRIVATE
    GTest::GTest
    common_utils
)

add_executable(test_image_resizer
    test-image-resizer.cpp
)

target_link_libraries(test_image_resizer
    PRIVATE
    GTest::GTest
    common_utils
    image_resizer)

add_test(NAME test_basic_base64 COMMAND $<TARGET_FILE:test_basic_base64>)
add_test(NAME test_image_resizer COMMAND $<TARGET_FILE:test_image_resizer>)
