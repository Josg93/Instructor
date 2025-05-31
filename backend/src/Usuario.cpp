#include "Usuario.hpp"

int Usuario::ultimoId = 0;

Usuario::Usuario(const std::string& nombre,const std::string email, const std::string& password)
    : id(++ultimoId), nombre(nombre),email(email),password(password) {}


int Usuario::getId() const { return id; }
std::string Usuario::getNombre() const { return nombre; }
std::string Usuario::getEmail() const {return email;}
std::string Usuario::getPassword() const { return password; }
void Usuario::setPassword(const std::string& nuevaPassword) { password = nuevaPassword; }