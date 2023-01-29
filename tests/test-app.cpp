#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <libasyik/service.hpp>
#include <libasyik/http.hpp>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "image_resizer/error.hpp"
#include "image_resizer/base64.hpp"
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

typedef std::tuple<uint16_t, std::string> HTTP_CODE;
uint16_t getCode(const HTTP_CODE &code)
{
    return std::get<0>(code);
}

std::string getReason(const HTTP_CODE &code)
{
    return std::get<1>(code);
}

HTTP_CODE validate_requests(const auto &req_ptr, rapidjson::Document &doc)
{

    if (req_ptr->headers["Content-Type"] != "application/json")
    {
        return std::make_tuple(415, "Content-Type error: payload must be defined as application/json");
    }

    else if (doc.Parse(req_ptr->body.c_str(), req_ptr->body.size()).HasParseError())
    {
        std::stringstream err;
        err << "JSON parse error: " << doc.GetParseError() << " - " << rapidjson::GetParseError_En(doc.GetParseError());
        return std::make_tuple(422, err.str());
    }

    else if (!doc.HasMember("input_jpeg"))
    {
        return std::make_tuple(400, "input_jpeg is not available in data.");
    }
    else if (!doc.HasMember("desired_width"))
    {
        return std::make_tuple(400, "desired_width is not available in data.");
    }
    else if (!doc.HasMember("desired_height"))
    {
        return std::make_tuple(400, "desired_height is not available in data.");
    }

    return std::make_tuple(200, "");
}

TEST(APP, main_func)
{
    auto as = asyik::make_service();
    auto server = asyik::make_http_server(as, "127.0.0.1", 8080);
    server->set_request_body_limit(10485760); // 10MB

    std::shared_ptr<ImageResizer> image_resizer = std::make_shared<ImageResizer>();

    // accept string argument
    server->on_http_request("/resize_image", "POST", [image_resizer](auto req, auto args)
                            {
                            HTTP_CODE val_code;
                            rapidjson::Document payload_data, payload_result;
                            rapidjson::StringBuffer buffer; buffer.Clear();
                            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

                            req->response.headers.set("Content-Type", "application/json");

                            val_code = validate_requests(req, payload_data);
                            if (getCode(val_code) != 200)
                            {
                              rapidjson::SetValueByPointer(payload_result, "/code", getCode(val_code));
                              rapidjson::SetValueByPointer(payload_result, "/Message", getReason(val_code).c_str());
                              payload_result.Accept(writer);
                              req->response.body = buffer.GetString();
                              req->response.result(getCode(val_code));
                            }
                            else
                            {
                              Error proc_code = image_resizer->process(payload_data, payload_result);

                              if (!proc_code.IsOk()) {
                                rapidjson::SetValueByPointer(payload_result, "/code", 500);
                                rapidjson::SetValueByPointer(payload_result, "/Message", proc_code.AsString().c_str());
                                payload_result.Accept(writer);
                                req->response.body = buffer.GetString();
                                req->response.result(500);
                              }
                              else {
                                rapidjson::SetValueByPointer(payload_result, "/code", 200);
                                rapidjson::SetValueByPointer(payload_result, "/message", "success");
                                payload_result.Accept(writer);
                                req->response.body = buffer.GetString();
                                req->response.result(200);
                              }
                            } });
    as->execute([as]() {
        auto req =
            asyik::http_easy_request(as, "POST", "http://127.0.0.1:8080/resize_image",
                                            "{\"desired_height\":[]}",
                                            {
                                                {"Content-Type", "text/html"}  //headers
                                            });

        EXPECT_TRUE(req->response.result() == 415);
        EXPECT_TRUE(req->response.body.compare(
            "{\"code\":415,\"Message\":\"Content-Type error: payload must be defined as application/json"));
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        req =
            asyik::http_easy_request(as, "POST", "http://127.0.0.1:8080/resize_image",
                                            "{[\"desired_height\":[]}",
                                            {
                                                {"Content-Type", "application/json"}  //headers
                                            });

        EXPECT_TRUE(req->response.result() == 422);
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        req = asyik::http_easy_request(
            as, "POST", "http://127.0.0.1:8080/resize_image",
            "{\"desired_height\":[]}",
            {
                {"Content-Type", "application/json"}  //headers
            });

        EXPECT_TRUE(req->response.result() == 400);
        EXPECT_TRUE(req->response.body.compare(
            "{\"code\":415,\"Message\":\"input_jpeg is not available in data."));
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        req = asyik::http_easy_request(
            as, "POST", "http://127.0.0.1:8080/resize_image",
            "{\"input_jpeg\": \"AAA\", \"desired_height\":[]}",
            {
                {"Content-Type", "application/json"}  //headers
            });

        EXPECT_TRUE(req->response.result() == 400);
        EXPECT_TRUE(req->response.body.compare(
            "{\"code\":415,\"Message\":\"desired_width is not available in data."));
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        req = asyik::http_easy_request(
            as, "POST", "http://127.0.0.1:8080/resize_image",
            "{\"input_jpeg\": \"AAA\", \"desired_width\":[]}",
            {
                {"Content-Type", "application/json"}  //headers
            });

        EXPECT_TRUE(req->response.result() == 400);
        EXPECT_TRUE(req->response.body.compare(
            "{\"code\":415,\"Message\":\"desired_height is not available in data."));
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        req = asyik::http_easy_request(
            as, "POST", "http://127.0.0.1:8080/resize_image",
            "{\"input_jpeg\": \"AAA??\", \"desired_width\": 720,  \"desired_height\": 480}",
            {
                {"Content-Type", "application/json"}  //headers
            });

        EXPECT_TRUE(req->response.result() == 500);
        EXPECT_TRUE(req->response.body.compare(
            "{\"code\":500,\"Message\":\"Failed: Input is not valid base64-encoded data."));
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        req = asyik::http_easy_request(
            as, "POST", "http://127.0.0.1:8080/resize_image",
            "{\"input_jpeg\": \"AAA\", \"desired_width\": 720,  \"desired_height\": 480}",
            {
                {"Content-Type", "application/json"}  //headers
            });

        EXPECT_TRUE(req->response.result() == 500);
        EXPECT_TRUE(req->response.body.compare(
            "{\"code\":500,\"Message\":\"Failed: String input is not a valid image encoded data."));
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        cv::Size mat_size{1280, 720};

        cv::Mat origin_image_test = cv::Mat(mat_size, CV_8UC3);
        std::string encoded_image_test = encode_image(origin_image_test, ".jpg");

        rapidjson::Document input_doc_test;
        rapidjson::Pointer("/input_jpeg").Set(input_doc_test, encoded_image_test.c_str());
        rapidjson::Pointer("/desired_width").Set(input_doc_test, 640);
        rapidjson::Pointer("/desired_height").Set(input_doc_test, 480);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        input_doc_test.Accept(writer);
        std::string input_str_test(buffer.GetString());

        req = asyik::http_easy_request(
            as, "POST", "http://127.0.0.1:8080/resize_image",
            input_str_test.c_str(),
            {
                {"Content-Type", "application/json"}  //headers
            });

        EXPECT_TRUE(req->response.result() == 200);
        EXPECT_TRUE(!req->response.headers["Content-Type"].compare("application/json"));

        rapidjson::Document output_doc_test;
        output_doc_test.Parse(req->response.body.c_str(), req->response.body.size());
        EXPECT_TRUE(output_doc_test.HasMember("output_jpeg"));
        EXPECT_TRUE(output_doc_test.HasMember("code"));
        EXPECT_TRUE(output_doc_test["code"].GetInt() == 200);
        EXPECT_TRUE(output_doc_test.HasMember("message"));
        EXPECT_STREQ(output_doc_test["message"].GetString(), "success");

        std::string output_img_str_test(output_doc_test["output_jpeg"].GetString());
        cv::Mat output_img_test = decode_image(output_img_str_test);
        EXPECT_TRUE((output_img_test.size() == cv::Size{640, 480}));

        as->stop();
    });

    as->run();
}