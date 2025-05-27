#pragma once
#include "Usuario.hpp"
#include "Estudiante.hpp"
#include "Profesor.hpp"
#include "FileDatabase.hpp"
#include <vector>
#include <memory>
#include <optional>


class UsuarioService {
private:
    static std::vector<std::shared_ptr<Usuario>> usuarios;
    static void loadUsuarios();
    static void saveUsuarios();

public:
    static bool registrarUsuario(const std::string& nombre, 
                               const std::string& password,
                               const std::string& tipo);
    
    static std::vector<std::shared_ptr<Usuario>> obtenerUsuarios();
    static std::optional<std::shared_ptr<Usuario>> obtenerPorId(int id);
    static std::optional<std::shared_ptr<Usuario>> autenticar(const std::string& nombre,const std::string& password);
    
    static std::vector<std::shared_ptr<Usuario>> listarUsuarios();
    static std::vector<std::shared_ptr<Profesor>> listarProfesores();
};