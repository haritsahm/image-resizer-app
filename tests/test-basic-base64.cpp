#include "image_resizer/base64.hpp"
#include <iostream>
#include <gtest/gtest.h>

TEST(Base64, basic)
{
    // Test all possibilites of fill bytes (none, one =, two ==)
    // References calculated with: https://www.base64encode.org/

    std::string rest0_original = "abc";
    std::string rest0_reference = "YWJj";

    std::string rest0_encoded = base64_encode(rest0_original);
    std::string rest0_decoded = base64_decode(rest0_encoded);

    ASSERT_EQ(rest0_decoded, rest0_original);

    std::cout << "encoded:   " << rest0_encoded << std::endl;
    std::cout << "reference: " << rest0_reference << std::endl;
    std::cout << "decoded:   " << rest0_decoded << std::endl
              << std::endl;

    std::string rest1_original = "abcd";
    std::string rest1_reference = "YWJjZA==";

    std::string rest1_encoded = base64_encode(rest1_original);
    std::string rest1_decoded = base64_decode(rest1_encoded);

    ASSERT_EQ(rest1_decoded, rest1_original);

    std::cout << "encoded:   " << rest1_encoded << std::endl;
    std::cout << "reference: " << rest1_reference << std::endl;
    std::cout << "decoded:   " << rest1_decoded << std::endl
              << std::endl;

    std::string rest2_original = "abcde";
    std::string rest2_reference = "YWJjZGU=";

    std::string rest2_encoded = base64_encode(rest2_original);
    std::string rest2_decoded = base64_decode(rest2_encoded);

    ASSERT_EQ(rest2_decoded, rest2_original);

    std::cout << "encoded:   " << rest2_encoded << std::endl;
    std::cout << "reference: " << rest2_reference << std::endl;
    std::cout << "decoded:   " << rest2_decoded << std::endl
              << std::endl;
}