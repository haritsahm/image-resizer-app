#ifndef IMAGE_RESIZER_HPP
#define IMAGE_RESIZER_HPP

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

class ImageResizer
{
public:
    ImageResizer() = default;
    ~ImageResizer(){};

    ImageResizer(const ImageResizer &obj) = delete;
    ImageResizer &operator=(const ImageResizer &obj);

    ImageResizer(ImageResizer &&obj) = delete;
    ImageResizer &operator=(ImageResizer &&obj);

    Error process(const rapidjson::Document &encoded_input, rapidjson::Document &encoded_output);
    Error process(const std::string &encoded_input, std::string &encoded_output);

private:
    /// @brief Decode image from base64 string data
    /// @param byte_string encoded string image
    /// @return Decoded image in cv::Mat format
    cv::Mat decode_image(const std::string &byte_string);

    /// @brief Encode cv::Mat image to base64 string data
    /// @param image input image to be encoded
    /// @param type Image encoding/compression type
    /// @return Encoded image in string format
    std::string encode_image(const cv::Mat &image, const std::string &type);
};

#endif