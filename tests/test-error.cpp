#include <gtest/gtest.h>
#include "image_resizer/error.hpp"

TEST(ERRORClass, error_class_handler){
    EXPECT_STREQ(Error::CodeString(Error::Code::SUCCESS).c_str(), "OK");
    EXPECT_STREQ(Error::CodeString(Error::Code::FAILED).c_str(), "FAILED");
    EXPECT_STREQ(Error::CodeString(Error::Code::UNKNOWN).c_str(), "<invalid code>");

    std::stringstream out;
    out << Error();
    EXPECT_TRUE(out.str().empty());

    out << Error(Error::Code::SUCCESS, "message");
    EXPECT_STREQ(out.str().c_str(), "message");
}