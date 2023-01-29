#include "image_resizer/image_resizer.hpp"

cv::Mat ImageResizer::decode_image(const std::string &byte_string)
{
    std::string decoded_img_str = base64_decode(byte_string);
    std::vector<uchar> data(decoded_img_str.begin(), decoded_img_str.end());
    cv::Mat image = cv::imdecode(data, cv::IMREAD_UNCHANGED);
    return image;
}

std::string ImageResizer::encode_image(const cv::Mat &image, const std::string &type)
{
    std::vector<uchar> buf;
    cv::imencode(type, image, buf);
    std::string buf_str(buf.begin(), buf.end());
    std::string encoded_img_str = base64_encode(buf_str);
    return encoded_img_str;
}

Error ImageResizer::process(const std::string &encoded_input_str, std::string &encoded_output_str)
{
    rapidjson::Document input_doc, output_doc;
    if (input_doc.Parse(encoded_input_str.c_str(), encoded_input_str.size()).HasParseError())
    {
        return Error(Error::Code::FAILED, "Unable to parse input str to json.");
    }

    if (!input_doc.HasMember("input_jpeg"))
    {
        return Error(Error::Code::FAILED, "input_jpeg is not available in data.");
    }

    else if (!input_doc.HasMember("desired_width"))
    {
        return Error(Error::Code::FAILED, "desired_width is not available in data.");
    }

    else if (!input_doc.HasMember("desired_height"))
    {
        return Error(Error::Code::FAILED, "desired_height is not available in data.");
    }

    Error res = process(input_doc, output_doc);
    if (!res.IsOk())
    {
        return res;
    }

    rapidjson ::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    output_doc.Accept(writer);
    encoded_output_str = std::string(buffer.GetString());

    return Error::Success;
}

Error ImageResizer::process(const rapidjson::Document &encoded_input_doc, rapidjson::Document &encoded_output_doc)
{
    cv::Mat decoded_image;
    try
    {
        std::string input_img_str(encoded_input_doc["input_jpeg"].GetString());
        decoded_image = decode_image(input_img_str);
    }
    catch (const std::runtime_error &err)
    {
        return Error(Error::Code::FAILED, err.what());
    }

    cv::Size resized_mat_size{encoded_input_doc["desired_width"].GetInt(), encoded_input_doc["desired_height"].GetInt()};

    if (decoded_image.empty())
        return Error(Error::Code::FAILED, "String input is not a valid image encoded data.");

    cv::Mat resized_image;
    cv::resize(decoded_image, resized_image, resized_mat_size, cv::INTER_NEAREST);

    std::string encoded_image_str = encode_image(resized_image, ".jpg");

    rapidjson::SetValueByPointer(encoded_output_doc, "/output_jpeg", encoded_image_str.c_str());

    return Error::Success;
}
