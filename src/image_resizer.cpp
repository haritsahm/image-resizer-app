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

Error ImageResizer::process(const std::string &encoded_input, std::string &encoded_output)
{
    cv::Mat decoded_image;
    try
    {
        decoded_image = decode_image(encoded_input);
    }
    catch (const std::runtime_error &err)
    {
        return Error(Error::Code::FAILED, err.what());
    }

    if (decoded_image.empty())
        return Error(Error::Code::FAILED, "String input is not a valid image encoded data.");

    cv::Size mat_size{1280, 720}; // dummy

    cv::Mat resized_image;
    cv::resize(decoded_image, resized_image, mat_size, cv::INTER_NEAREST);

    encoded_output = encode_image(resized_image, ".jpg");

    return Error::Success;
}