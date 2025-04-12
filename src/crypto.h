#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>

class Crypto {
public:
    // Default constructor uses the secret managed internally.
    Crypto() : secret(getSecretFromDisk()) {}

    // Alternate constructor lets you override the secret.
    explicit Crypto(std::string secretKey) : secret(std::move(secretKey)) {}

    // Static methods for base64 encoding and decoding.
    static std::string encodeBase64(const std::string& input);
    static std::string decodeBase64(const std::string& input);

    // Signs the given input using HMAC-SHA256 and returns the signature as base64.
    [[nodiscard]] std::string sign(const std::string& input) const;

    // Manages the secret: returns the secret from disk or creates one if not existing.
    static std::string getSecretFromDisk(const std::string& path = "secret.txt");

private:
    std::string secret;

    // Generates a new random 32-byte hexadecimal secret.
    static std::string generateSecret();
};

#endif // CRYPTO_H