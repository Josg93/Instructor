#pragma once
#include <jwt-cpp/jwt.h>
#include <drogon/drogon.h>

class JWT {
public:
    static std::optional<int> validarToken(const HttpRequestPtr& req) {
        try {
            auto authHeader = req->getHeader("Authorization");
            if (authHeader.empty() || authHeader.find("Bearer ") != 0) {
                return std::nullopt;
            }
            
            auto token = authHeader.substr(7); // Elimina "Bearer "
            auto decoded = jwt::decode(token);
            
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{"ROPAINTERIOR"})
                .with_issuer("auth0");
            
            verifier.verify(decoded);
            return std::stoi(decoded.get_payload_claim("id").as_string());
        } catch (...) {
            return std::nullopt;
        }
    }
};