// Base64 string decoder
// Source: https://stackoverflow.com/a/13935718
// Source: http://www.sunshine2k.de/articles/coding/base64/understanding_base64.html

#ifndef BASE64_HPP
#define BASE64_HPP

#include <string>
#include <vector>

/// @brief Decode encoded base64 string
/// @param encoded_string string to decode
/// @return Decoded string data
std::string base64_decode(std::string const &encoded_string);

/// @brief Encode raw string
/// @param encoded_string string to encode
/// @return Encoded string data
std::string base64_encode(std::string const &s);

#endif