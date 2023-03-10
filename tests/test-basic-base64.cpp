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

    std::string rest3_original = "abcde1234";
    std::string rest3_reference = "YWJjZGUxMjM0";

    std::string rest3_encoded = base64_encode(rest3_original);
    std::string rest3_decoded = base64_decode(rest3_reference);

    ASSERT_EQ(rest3_decoded, rest3_original);

    std::cout << "encoded:   " << rest3_encoded << std::endl;
    std::cout << "reference: " << rest3_reference << std::endl;
    std::cout << "decoded:   " << rest3_decoded << std::endl
              << std::endl;

    std::string rest4_original = "abcde1234+";
    std::string rest4_reference = "YWJjZGUxMjM0Kw==";

    std::string rest4_encoded = base64_encode(rest4_original);
    std::string rest4_decoded = base64_decode(rest4_reference);

    ASSERT_EQ(rest4_decoded, rest4_original);

    std::cout << "encoded:   " << rest4_encoded << std::endl;
    std::cout << "reference: " << rest4_reference << std::endl;
    std::cout << "decoded:   " << rest4_decoded << std::endl
              << std::endl;

    std::string rest5_original = "abcde1234/";
    std::string rest5_reference = "YWJjZGUxMjM0Lw==";

    std::string rest5_encoded = base64_encode(rest5_original);
    std::string rest5_decoded = base64_decode(rest5_reference);

    ASSERT_EQ(rest5_decoded, rest5_original);

    std::cout << "encoded:   " << rest5_encoded << std::endl;
    std::cout << "reference: " << rest5_reference << std::endl;
    std::cout << "decoded:   " << rest5_decoded << std::endl
              << std::endl;

    std::string rest6_original = "abcde1234+/";
    std::string rest6_reference = "YWJjZGUxMjM0Ky8=";

    std::string rest6_encoded = base64_encode(rest6_original);
    std::string rest6_decoded = base64_decode(rest6_reference);

    ASSERT_EQ(rest6_decoded, rest6_original);

    std::cout << "encoded:   " << rest6_encoded << std::endl;
    std::cout << "reference: " << rest6_reference << std::endl;
    std::cout << "decoded:   " << rest6_decoded << std::endl
              << std::endl;

    std::string rest7_original = "123456";
    std::string rest7_reference = "MTIzNDU2";

    std::string rest7_encoded = base64_encode(rest7_original);
    std::string rest7_decoded = base64_decode(rest7_reference);

    ASSERT_EQ(rest7_decoded, rest7_original);

    std::cout << "encoded:   " << rest7_encoded << std::endl;
    std::cout << "reference: " << rest7_reference << std::endl;
    std::cout << "decoded:   " << rest7_decoded << std::endl
              << std::endl;

    std::string rest8_original = "";
    std::string rest8_reference = "";

    std::string rest8_encoded = base64_encode(rest8_original);
    std::string rest8_decoded = base64_decode(rest8_reference);

    ASSERT_EQ(rest8_decoded, rest8_original);

    std::cout << "encoded:   (empty)" << rest8_encoded << std::endl;
    std::cout << "reference: (empty)" << rest8_reference << std::endl;
    std::cout << "decoded:   (empty)" << rest8_decoded << std::endl
              << std::endl;
}

TEST(Base64, invalid_encoded)
{
    // Test with input outside base64_chars

    std::string rest0_original = "YWJ2Z?GMyMw==";

    EXPECT_THROW(base64_decode(rest0_original), std::runtime_error);

    try
    {
        base64_decode(rest0_original);
    }
    catch (std::runtime_error const &err)
    {
        EXPECT_EQ(err.what(), std::string("Input is not valid base64-encoded data."));
    }
}
