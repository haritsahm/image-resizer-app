include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test_error_class
    test-basic-base64.cpp
)
target_link_libraries(test_error_class
    PRIVATE
    GTest::GTest
    common_utils
)

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

add_executable(test_rapid_json
    test-rapidjson-parser.cpp
)

target_link_libraries(test_rapid_json
    PRIVATE
    GTest::GTest
    common_utils)
target_include_directories(test_rapid_json PRIVATE ${RapidJSON_INCLUDE_DIRS})

add_executable(test_app
    test-app.cpp
)

target_link_libraries(test_app
    PRIVATE
    GTest::GTest
    common_utils
    image_resizer
    libasyik)
target_include_directories(test_app PRIVATE ${RapidJSON_INCLUDE_DIRS} ${libasyik_INCLUDE_DIR})

add_test(NAME test_error_class COMMAND $<TARGET_FILE:test_error_class>)
add_test(NAME test_basic_base64 COMMAND $<TARGET_FILE:test_basic_base64>)
add_test(NAME test_image_resizer COMMAND $<TARGET_FILE:test_image_resizer>)
add_test(NAME test_rapid_json COMMAND $<TARGET_FILE:test_rapid_json>)
add_test(NAME test_app COMMAND $<TARGET_FILE:test_app>)
