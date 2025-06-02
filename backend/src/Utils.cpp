#include "Utils.hpp"

namespace Utils {
    std::string hashPassword(const std::string& password) {
        unsigned char hash[EVP_MAX_MD_SIZE];
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        const EVP_MD* md = EVP_sha256();

        EVP_DigestInit_ex(mdctx, md, NULL);
        EVP_DigestUpdate(mdctx, password.c_str(), password.length());
        EVP_DigestFinal_ex(mdctx, hash, NULL);
        EVP_MD_CTX_free(mdctx);

        std::stringstream ss;
        for(int i = 0; i < EVP_MD_size(md); i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }

        return ss.str();
    }

    bool verifyPassword(const std::string& password, const std::string& hash) {
        return hashPassword(password) == hash;
    }

    bool validarEmail(const std::string& email) {
        const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        return std::regex_match(email, pattern);
    }
}