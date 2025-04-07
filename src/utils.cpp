#include "utils.h"

std::string Utils::toLowerCase(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

MimeType Utils::getMimeType(const std::string& filename) {
    const auto pos = filename.rfind('.');
    if (pos == std::string::npos)
        return MimeType::TXT; // Default to text/plain if no extension is found
    const std::string ext = toLowerCase(filename.substr(pos + 1));
    static const std::unordered_map<std::string, MimeType> mimeTypes = {
            {"html", MimeType::HTML},
            {"htm", MimeType::HTM},
            {"css", MimeType::CSS},
            {"js", MimeType::JS},
            {"png", MimeType::PNG},
            {"jpg", MimeType::JPG},
            {"jpeg", MimeType::JPEG},
            {"gif", MimeType::GIF},
            {"webp", MimeType::WEBP},
            {"txt", MimeType::TXT},
            {"json", MimeType::JSON},
            {"pdf", MimeType::PDF},
            {"webm", MimeType::WEBM}
        };
    const auto it = mimeTypes.find(ext);
    return it != mimeTypes.end() ? it->second : MimeType::TXT; // Default to text/plain if extension is not found
}

std::string Utils::toMimeString(const MimeType& mimeType) {
    switch (mimeType) {
    case MimeType::HTML:
    case MimeType::HTM:
        return "text/html";
    case MimeType::CSS:
        return "text/css";
    case MimeType::JS:
        return "text/javascript";
    case MimeType::PNG:
        return "image/png";
    case MimeType::JPG:
    case MimeType::JPEG:
        return "image/jpeg";
    case MimeType::GIF:
        return "image/gif";
    case MimeType::WEBP:
        return "image/webp";
    case MimeType::TXT:
        return "text/plain";
    case MimeType::JSON:
        return "application/json";
    case MimeType::PDF:
        return "application/pdf";
    case MimeType::WEBM:
        return "video/webm";
    default:
        return "application/octet-stream";
    }
}
