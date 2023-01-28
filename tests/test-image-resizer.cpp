#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "image_resizer/base64.hpp"

// TDD since its functionalities will be private
cv::Mat decode_image(const std::string &byte_string)
{
    std::string decoded_img_str = base64_decode(byte_string);
    std::vector<uchar> data(decoded_img_str.begin(), decoded_img_str.end());
    cv::Mat image = cv::imdecode(data, cv::IMREAD_UNCHANGED);
    return image;
}

std::string encode_image(const cv::Mat &image, const std::string &type)
{
    std::vector<uchar> buf;
    cv::imencode(type, image, buf);
    std::string buf_str(buf.begin(), buf.end());
    std::string encoded_img_str = base64_encode(buf_str);
    return encoded_img_str;
}

TEST(ImageResizer, image_encode_decode)
{
    // Encode image from zeros, ones, random
    cv::Size mat_size{1280, 720};

    cv::Mat origin_image_test1 = cv::Mat::zeros(mat_size, CV_8UC1);
    std::string encoded_image_test1 = encode_image(origin_image_test1, ".jpg");
    cv::Mat decoded_image_test1 = decode_image(encoded_image_test1);
    cv::Mat comparator_test1;
    cv::bitwise_xor(decoded_image_test1, origin_image_test1, comparator_test1);

    ASSERT_TRUE(cv::countNonZero(comparator_test1.reshape(1)) == 0);

    cv::Mat origin_image_test2 = cv::Mat::zeros(mat_size, CV_8UC3);
    std::string encoded_image_test2 = encode_image(origin_image_test2, ".png");
    cv::Mat decoded_image_test2 = decode_image(encoded_image_test2);
    cv::Mat comparator_test2;
    cv::bitwise_xor(decoded_image_test2, origin_image_test2, comparator_test2);

    ASSERT_TRUE(cv::countNonZero(comparator_test2.reshape(1)) == 0);

    std::string encoded_string_test3{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"
        "LCBub3QgYmFzZTY0IGVuY29kaW5nKS4gQ2Vyd"
        "GFpbmx5LCB3ZSBjYW4gZG8gdGhpcyBieSBzYX"
        "ZpbmcgYW5kIHJlYWRpbmcgdGhlIGltYWdlIHdp"
        "dGggaW1zYXZlIGFuZCBpbXJlYWQsIGJ1dCBQSU"
        "wgcHJvdmlkZXMgYSBtb3JlIGRpcmVjdCBtZXRob2Q6"};
    cv::Mat decoded_image_test3 = decode_image(encoded_string_test3);
    ASSERT_TRUE(decoded_image_test3.empty());
}