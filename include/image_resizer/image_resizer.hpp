#ifndef IMAGE_RESIZER_HPP
#define IMAGE_RESIZER_HPP

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
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

    Error process(const std::string &encoded_input, std::string &encoded_output);

private:
    cv::Mat decode_image(const std::string &byte_string);
    std::string encode_image(const cv::Mat &image, const std::string &type);
};

#endif