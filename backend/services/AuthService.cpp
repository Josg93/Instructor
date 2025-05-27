#include "AuthService.hpp"
#include "UsuarioService.hpp"
#include <jwt-cpp/jwt.h>

std::optional<std::string> AuthService::login(const std::string& nombre, const std::string& password) {
    auto usuarios = UsuarioService::obtenerUsuarios();
    for (const auto& u : usuarios) {
        if (u->getNombre() == nombre && u->getPassword() == password) { 
            // Generar token JWT
            auto token = jwt::create()
                .set_issuer("InstructorApp")
                .set_type("JWS")
                .set_payload_claim("id", jwt::claim(std::to_string(u->getId())))
                .set_payload_claim("tipo", jwt::claim(u->getTipo()))  // Usamos el método virtual
                .sign(jwt::algorithm::hs256{"secreto"});
            return token;
        }
    }
    return std::nullopt;
}