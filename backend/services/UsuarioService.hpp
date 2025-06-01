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
                               const std::string& email,
                               const std::string& tipo);
                               
    static bool eliminarUsuario(int usuarioId);
    static std::vector<std::shared_ptr<Usuario>> obtenerUsuarios();
    static std::optional<std::shared_ptr<Usuario>> obtenerPorId(int id);
    static std::optional<std::shared_ptr<Usuario>> obtenerPorEmail(std::string& email);
    static std::optional<std::shared_ptr<Usuario>> autenticar(const std::string& nombre,const std::string& email ,const std::string& password);

    static bool actualizarUsuario(int usuarioId,
                                   std::optional<std::string> nuevoNombre,
                                   std::optional<std::string> nuevoEmail,
                                   std::optional<std::string> nuevaPassword);
    
    static std::vector<std::shared_ptr<Usuario>> listarUsuarios();
    static std::vector<std::shared_ptr<Profesor>> listarProfesores();
};