// crypto.cpp
#include "crypto.h"

#include <filesystem>
namespace fs = std::filesystem;

std::string Crypto::encodeBase64(const std::string& input) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.data(), static_cast<int>(input.size()));
    BIO_flush(bio);

    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return encoded;
}

std::string Crypto::decodeBase64(const std::string& input) {
    BIO* bio = BIO_new_mem_buf(input.data(), static_cast<int>(input.size()));
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    std::vector<char> buffer(input.size());
    const int decodedLength = BIO_read(bio, buffer.data(), static_cast<int>(input.size()));
    BIO_free_all(bio);
    if (decodedLength > 0)
        return std::string{buffer.data(), static_cast<size_t>(decodedLength)};
    return "";
}

std::string Crypto::sign(const std::string& input) const {
    unsigned char hmacBuffer[EVP_MAX_MD_SIZE] = {};
    unsigned int hmacLength = 0;
    HMAC(EVP_sha256(),
         secret.data(), static_cast<int>(secret.size()),
         reinterpret_cast<const unsigned char*>(input.data()), input.size(),
         hmacBuffer, &hmacLength);

    const std::string rawSignature(reinterpret_cast<char*>(hmacBuffer), hmacLength);
    return encodeBase64(rawSignature);
}

std::string Crypto::getSecretFromDisk(const std::string& path) {
    if (fs::exists(path)) {
        std::ifstream inFile(path);
        std::string secret;
        std::getline(inFile, secret);
        return secret;
    }
    std::string secret = generateSecret();
    std::ofstream outFile(path);
    outFile << secret;
    return secret;
}

std::string Crypto::generateSecret() {
    std::random_device rd;
    std::uniform_int_distribution dist(0, 255);
    std::ostringstream oss;
    for (int i = 0; i < 32; i++) { // generate 32 random bytes
        oss << std::hex << std::setw(2) << std::setfill('0') << dist(rd);
    }
    return oss.str();
}