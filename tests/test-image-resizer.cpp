#include <iostream>
#include <gtest/gtest.h>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "image_resizer/base64.hpp"
#include "image_resizer/error.hpp"
#include "image_resizer/image_resizer.hpp"

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

TEST(ImageResizerFunc, image_encode_decode)
{
    // Encode image from zeros, ones, random
    cv::Size mat_size{1280, 720};

    cv::Mat origin_image_test1 = cv::Mat::zeros(mat_size, CV_8UC1);
    std::string encoded_image_test1 = encode_image(origin_image_test1, ".jpg");
    cv::Mat decoded_image_test1 = decode_image(encoded_image_test1);
    cv::Mat comparator_test1;
    cv::bitwise_xor(decoded_image_test1, origin_image_test1, comparator_test1);

    EXPECT_TRUE(cv::countNonZero(comparator_test1.reshape(1)) == 0);

    cv::Mat origin_image_test2 = cv::Mat::zeros(mat_size, CV_8UC3);
    std::string encoded_image_test2 = encode_image(origin_image_test2, ".png");
    cv::Mat decoded_image_test2 = decode_image(encoded_image_test2);
    cv::Mat comparator_test2;
    cv::bitwise_xor(decoded_image_test2, origin_image_test2, comparator_test2);

    EXPECT_TRUE(cv::countNonZero(comparator_test2.reshape(1)) == 0);

    cv::Mat origin_image_test3 = cv::Mat(mat_size, CV_32FC3);
    std::string encoded_image_test3 = encode_image(origin_image_test3, ".png");
    cv::Mat decoded_image_test3 = decode_image(encoded_image_test3);

    EXPECT_TRUE(!decoded_image_test3.empty() && decoded_image_test3.size() == mat_size);

    std::string encoded_string_test4{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"
        "LCBub3QgYmFzZTY0IGVuY29kaW5nKS4gQ2Vyd"
        "GFpbmx5LCB3ZSBjYW4gZG8gdGhpcyBieSBzYX"
        "ZpbmcgYW5kIHJlYWRpbmcgdGhlIGltYWdlIHdp"
        "dGggaW1zYXZlIGFuZCBpbXJlYWQsIGJ1dCBQSU"
        "wgcHJvdmlkZXMgYSBtb3JlIGRpcmVjdCBtZXRob2Q6"};
    cv::Mat decoded_image_test4 = decode_image(encoded_string_test4);
    EXPECT_TRUE(decoded_image_test4.empty());
}

TEST(ImageResizerFunc, resizer_class_proc_str)
{
    ImageResizer image_resizer_obj;
    cv::Size mat_size{1280, 720};

    std::string input_str_test1{"{\"foo\":[123]}"};
    std::string output_str_test1;
    Error res_test1 = image_resizer_obj.process(input_str_test1, output_str_test1);
    EXPECT_EQ(res_test1, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test1.Message().c_str(), "input_jpeg is not available in data.");

    std::string input_str_test2{"{\"input_jpeg\": \"AAAA\", \"desired_width\": 640}"};
    std::string output_str_test2;
    Error res_test2 = image_resizer_obj.process(input_str_test2, output_str_test2);
    EXPECT_EQ(res_test2, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test2.Message().c_str(), "desired_height is not available in data.");

    std::string input_str_test3{"{\"input_jpeg\": \"AAAA\", \"desired_height\": 480}"};
    std::string output_str_test3;
    Error res_test3 = image_resizer_obj.process(input_str_test3, output_str_test3);
    EXPECT_EQ(res_test3, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test3.Message().c_str(), "desired_width is not available in data.");

    cv::Mat origin_image_test4 = cv::Mat(mat_size, CV_8UC3);
    std::string encoded_image_test4 = encode_image(origin_image_test4, ".jpg");

    rapidjson::Document input_doc_test4;
    rapidjson::Pointer("/input_jpeg").Set(input_doc_test4, encoded_image_test4.c_str());
    rapidjson::Pointer("/desired_width").Set(input_doc_test4, 640);
    rapidjson::Pointer("/desired_height").Set(input_doc_test4, 480);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    input_doc_test4.Accept(writer);
    std::string input_str_test4(buffer.GetString());

    std::string output_str_test4;
    Error res_test4 = image_resizer_obj.process(input_str_test4, output_str_test4);
    EXPECT_EQ(res_test4, Error::Success);

    rapidjson::Document output_doc_test4;
    output_doc_test4.Parse(output_str_test4.c_str(), output_str_test4.size());
    EXPECT_TRUE(output_doc_test4.HasMember("output_jpeg"));

    cv::Mat output_img_test4 = decode_image(output_doc_test4["output_jpeg"].GetString());
    EXPECT_TRUE((output_img_test4.size() == cv::Size{640, 480}));

    std::string encoded_image_test5{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"};

    rapidjson::Document input_doc_test5;
    rapidjson::Pointer("/input_jpeg").Set(input_doc_test5, encoded_image_test5.c_str());
    rapidjson::Pointer("/desired_width").Set(input_doc_test5, 640);
    rapidjson::Pointer("/desired_height").Set(input_doc_test5, 480);

    buffer.Clear();
    writer.Reset(buffer);
    input_doc_test5.Accept(writer);
    std::string input_str_test5(buffer.GetString());

    std::string output_str_test5;
    Error res_test5 = image_resizer_obj.process(input_str_test5, output_str_test5);
    EXPECT_EQ(res_test5, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test5.Message().c_str(), "String input is not a valid image encoded data.");

    std::string encoded_err_test6{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"
        "LCBub3QgYmFzZTY0IGVuY29kaW5nKS4gQ2Vyd"
        "GFpbmx5LCB3ZSBjYW4gZG8gdGhpcyBieSBzYX"
        "ZpbmcgYW5kIHJlYWRpbmcgdGhlIGltYWdlIHdp"
        "dGggaW1zYXZlIGFuZCBpbXJlYWQsIGJ1dCBQSU"
        "wgcHJvdmlkZXMgYSBtb3JlIGRpcm?jdCBtZXR$?ob2Q6"};

    rapidjson::Document input_doc_test6;
    rapidjson::Pointer("/input_jpeg").Set(input_doc_test6, encoded_err_test6.c_str());
    rapidjson::Pointer("/desired_width").Set(input_doc_test6, 640);
    rapidjson::Pointer("/desired_height").Set(input_doc_test6, 480);

    buffer.Clear();
    writer.Reset(buffer);
    input_doc_test6.Accept(writer);
    std::string input_str_test6(buffer.GetString());

    std::string output_str_test6;
    Error res_test6 = image_resizer_obj.process(input_str_test6, output_str_test6);
    EXPECT_EQ(res_test6, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test6.Message().c_str(), "Input is not valid base64-encoded data.");
}

TEST(ImageResizerFunc, resizer_class_proc_json)
{
    ImageResizer image_resizer_obj;
    cv::Size mat_size{1280, 720};

    rapidjson::Document input_doc_test1, output_doc_test1;
    input_doc_test1.Parse("{\"input_jpeg\": \"AAA?A\", \"desired_width\": 640, \"desired_height\": 480}");

    Error res_test1 = image_resizer_obj.process(input_doc_test1, output_doc_test1);
    EXPECT_EQ(res_test1, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test1.Message().c_str(), "Input is not valid base64-encoded data.");

    rapidjson::Document input_doc_test2, output_doc_test2;
    input_doc_test2.Parse("{\"input_jpeg\": \"AAAA?\", \"desired_width\": 640, \"desired_height\": 480}");

    cv::Mat origin_image_test2 = cv::Mat(mat_size, CV_8UC3);
    std::string encoded_image_test2 = encode_image(origin_image_test2, ".jpg");
    rapidjson::Pointer("/input_jpeg").Set(input_doc_test2, encoded_image_test2.c_str());

    Error res_test2 = image_resizer_obj.process(input_doc_test2, output_doc_test2);
    EXPECT_EQ(res_test2, Error::Success);
    EXPECT_TRUE(output_doc_test2.HasMember("output_jpeg"));

    std::string output_img_str_test2(output_doc_test2["output_jpeg"].GetString());
    cv::Mat output_img_test2 = decode_image(output_img_str_test2);
    EXPECT_TRUE((output_img_test2.size() == cv::Size{640, 480}));

    std::string encoded_image_test3{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"};
    rapidjson::Document input_doc_test3, output_doc_test3;
    input_doc_test3.Parse("{\"input_jpeg\": \"AAAA?\", \"desired_width\": 640, \"desired_height\": 480}");
    rapidjson::Pointer("/input_jpeg").Set(input_doc_test3, encoded_image_test3.c_str());

    Error res_test3 = image_resizer_obj.process(input_doc_test3, output_doc_test3);
    EXPECT_EQ(res_test3, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test3.Message().c_str(), "String input is not a valid image encoded data.");

    std::string encoded_err_test4{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"
        "LCBub3QgYmFzZTY0IGVuY29kaW5nKS4gQ2Vyd"
        "GFpbmx5LCB3ZSBjYW4gZG8gdGhpcyBieSBzYX"
        "ZpbmcgYW5kIHJlYWRpbmcgdGhlIGltYWdlIHdp"
        "dGggaW1zYXZlIGFuZCBpbXJlYWQsIGJ1dCBQSU"
        "wgcHJvdmlkZXMgYSBtb3JlIGRpcm?jdCBtZXR$?ob2Q6"};
    rapidjson::Document input_doc_test4, output_doc_test4;
    input_doc_test4.Parse("{\"input_jpeg\": \"AAAA?\", \"desired_width\": 640, \"desired_height\": 480}");
    rapidjson::Pointer("/input_jpeg").Set(input_doc_test4, encoded_err_test4.c_str());

    Error res_test4 = image_resizer_obj.process(input_doc_test4, output_doc_test4);
    EXPECT_EQ(res_test4, Error(Error::Code::FAILED));
    EXPECT_STREQ(res_test4.Message().c_str(), "Input is not valid base64-encoded data.");
}

TEST(ImageResizerFunc, failed_image_encode)
{
    std::string encoded_str_err{
        "VGhlIGtleSBwb2ludCBpcyBob3cgdG8gY2"
        "9udmVydCBhIG51bXB5IGFycmF5IHRvIGJ5d"
        "GVzIG9iamVjdCB3aXRoIGVuY29kaW5nIChz"
        "dWNoIGFzIEpQRUcgb3IgUE5HIGVuY29kaW5n"
        "LCBub3QgYmFzZTY0IGVuY29kaW5nKS4gQ2Vyd"
        "GFpbmx5LCB3ZSBjYW4gZG8gdGhpcyBieSBzYX"
        "ZpbmcgYW5kIHJlYWRpbmcgdGhlIGltYWdlIHdp"
        "dGggaW1zYXZlIGFuZCBpbXJlYWQsIGJ1dCBQSU"
        "wgcHJvdmlkZXMgYSBtb3JlIGRpcmVjdCBtZXR?ob2Q6"};

    EXPECT_THROW(decode_image(encoded_str_err), std::runtime_error);
}
