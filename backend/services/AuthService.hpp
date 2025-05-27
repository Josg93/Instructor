#pragma once
#include <optional>
#include <string>
#include "Usuario.hpp"

class AuthService {
public:
    static std::optional<std::string> login(const std::string& nombre, const std::string& password);
};