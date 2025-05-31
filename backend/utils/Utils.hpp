#include <bcrypt/BCrypt.hpp>
#include <string>

namespace Utils {
    std::string hashPassword(const std::string& password) 
    {
        return BCrypt::generateHash(password);
    }

    bool verifyPassword(const std::string& password, const std::string& hash) 
    {
        return BCrypt::validatePassword(password, hash);
    }
}