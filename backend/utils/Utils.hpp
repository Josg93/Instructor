#pragma once
#include <string>
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>

namespace Utils {
    // Declaraciones solamente
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
}