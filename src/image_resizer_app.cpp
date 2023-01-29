#include <libasyik/service.hpp>
#include <libasyik/http.hpp>
#include <iostream>
#include <typeinfo>
#include <memory>
#include <string>
#include <sstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <image_resizer/error.hpp>
#include "image_resizer/image_resizer.hpp"

typedef std::tuple<uint16_t, std::string> HTTP_CODE;
uint16_t &getCode(const HTTP_CODE &code)
{
    return std::get<0>(code);
}

std::string &getReason(const HTTP_CODE &code)
{
    return std::get<1>(code);
}

HTTP_CODE validate_requests(const auto &req_ptr, rapidjson::Document &doc)
{

    if (req_ptr->headers["Content-Type"] != "application/json")
    {
        return std::make_tuple(415, "Content type error: payload must be defined as application/json");
    }

    if (doc.Parse(req_ptr->body.c_str(), req_ptr->body.size()).HasParseError())
    {
        std::stringstream err;
        err << "JSON parse error: " << doc.GetParseError() << " - " << rapidjson::GetParseError_En(doc.GetParseError());
        return std::make_tuple(415, err.str());
    }

    if (!doc.HasMember("input_jpeg"))
    {
        return std::make_tuple(422, "input_jpeg is not available in data.");
    }
    else if (!doc.HasMember("desired_width"))
    {
        return std::make_tuple(422, "desired_width is not available in data.");
    }
    else if (!doc.HasMember("desired_height"))
    {
        return std::make_tuple(422, "desired_height is not available in data.");
    }

    return std::make_tuple(200, "");
}

int main()
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

                            val_code = validate_requests(req, payload_data);
                            if (getCode(val_code) != 200)
                            {
                              req->response.headers.set("Content-Type", "application/json");
                              req->response.result(getCode(val_code));
                            }
                            else
                            {
                              Error proc_code = image_resizer->process(payload_data, payload_result);
                              rapidjson::StringBuffer buffer; buffer.Clear();
                              rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                              payload_result.Accept(writer);

                              if (!proc_code.IsOk()) {
                                rapidjson::SetValueByPointer(payload_result, "/code", 422);
                                rapidjson::SetValueByPointer(payload_result, "/Message", proc_code.AsString());
                                req->response.result(422);
                                req->response.body = buffer.GetString();
                              }
                              else {
                                rapidjson::SetValueByPointer(payload_result, "/code", 200);
                                rapidjson::SetValueByPointer(payload_result, "/message", "success");

                                req->response.body = buffer.GetString();
                                req->response.headers.set("Content-Type", "application/json");
                                req->response.result(200);
                              }
                            } });

    as->run();

    return 0;
}