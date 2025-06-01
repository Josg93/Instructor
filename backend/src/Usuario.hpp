// Usuario.hpp
#pragma once
#include <string>
#include <vector>
#include <memory>

class Usuario {
protected:
    int id;
    std::string nombre;
    std::string email;
    std::string password;
    static int ultimoId;

public:
    Usuario(const std::string& nombre, const std::string& email, const std::string& password);
    virtual ~Usuario() = default;

    // Getters
    int getId() const;
    std::string getNombre() const;
    std::string getEmail() const;
    std::string getPassword() const;

    // Setters
    void setPassword(const std::string& nuevaPassword);
    void setEmail(const std::string& nuevoEmail);
    void setId(int id_){ this->id = id_;}
    void setNombre(const std::string& nuevoNombre) { nombre = nuevoNombre; }
    
    // Método virtual para el tipo de usuario
    virtual std::string getTipo() const = 0;
};