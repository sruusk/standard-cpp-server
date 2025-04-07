#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <unordered_map>
#include <algorithm>

enum class MimeType {
    HTML,
    HTM,
    CSS,
    JS,
    PNG,
    JPG,
    JPEG,
    GIF,
    WEBP,
    TXT,
    JSON,
    PDF,
    WEBM
};

class Utils {
public:
    static std::string toLowerCase(std::string s);

    static MimeType getMimeType(const std::string& filename);
    static std::string toMimeString(const MimeType& mimeType);
};
#endif //UTILS_H
