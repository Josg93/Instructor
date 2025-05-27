#include "UsuarioService.hpp"
#include "Estudiante.hpp"
#include "Profesor.hpp"
#include <sstream>

std::vector<std::shared_ptr<Usuario>> UsuarioService::usuarios;


//------------------METODOS PARA GUARDAR EN TXT --------------------------------
void UsuarioService::loadUsuarios() {
    auto parser = [](const std::string& line) -> std::shared_ptr<Usuario> {
        std::istringstream ss(line);
        std::string tipo, nombre, password;
        std::getline(ss, tipo, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, password);
        
        if (tipo == "profesor") {
            return std::make_shared<Profesor>(nombre, password);
        } else {
            return std::make_shared<Estudiante>(nombre, password);
        }
    };
    
    usuarios = FileDatabase::loadAll<std::shared_ptr<Usuario>>("usuarios.txt", parser);
}

void UsuarioService::saveUsuarios() {
    auto serializer = [](const std::shared_ptr<Usuario>& u) -> std::string {
        return u->getTipo() + "," + u->getNombre() + "," + u->getPassword();
    };
    
    FileDatabase::saveAll<std::shared_ptr<Usuario>>("usuarios.txt", usuarios, serializer);
}
//-------------------------------------------------------------------------------------------



bool UsuarioService::registrarUsuario(const std::string& nombre, const std::string& password, const std::string& tipo) {
    if (usuarios.empty()) loadUsuarios();
    
    for (const auto& u : usuarios) {
        if (u->getNombre() == nombre) return false;
    }
    
    std::shared_ptr<Usuario> nuevo;
    if (tipo == "profesor") {
        nuevo = std::make_shared<Profesor>(nombre, password);
    } else {
        nuevo = std::make_shared<Estudiante>(nombre, password);
    }
    
    usuarios.push_back(nuevo);
    saveUsuarios();
    return true;
}


//BUSCA ENTRE LOS USUARIOS POR ID
std::optional<std::shared_ptr<Usuario>> UsuarioService::obtenerPorId(int id) 
{
    if(UsuarioService::usuarios.empty())
    {
        return std::nullopt;
    }

    auto it = std::find_if(usuarios.begin(), usuarios.end(), [id](const auto& u) 
    {return u->getId() == id; });

    
    return it != usuarios.end() ? std::make_optional(*it) : std::nullopt;
    //si iterador es distinto del final de usuarios, true: enviamos como punter it, false: no mandas un carrizo
}



//funcion de autenticacion, VERIFICA QUE EL USUARIO ESTÉ INSCRITO
std::optional<std::shared_ptr<Usuario>> UsuarioService::autenticar(const std::string& nombre, const std::string& password) {
    auto it = std::find_if(usuarios.begin(), usuarios.end(),[&](const auto& u) 
        { 
            return u->getNombre() == nombre && u->getPassword() == password; 
        });
    return it != usuarios.end() ? std::make_optional(*it) : std::nullopt;
}

std::vector<std::shared_ptr<Usuario>> UsuarioService::listarUsuarios() {
    return usuarios;
}


//wtf
std::vector<std::shared_ptr<Profesor>> UsuarioService::listarProfesores() {
    std::vector<std::shared_ptr<Profesor>> profesores;
    for (const auto& u : usuarios) {
        if (auto p = std::dynamic_pointer_cast<Profesor>(u)) {
            profesores.push_back(p);
        }
    }
    return profesores;
}



/*#include "UsuarioService.hpp"
#include <algorithm>

std::vector<std::shared_ptr<Usuario>> UsuarioService::usuarios;


std::vector<std::shared_ptr<Usuario>> UsuarioService::obtenerUsuarios() {
    return usuarios;
}

//REGISTRAR USUARIOS MANITO
bool UsuarioService::registrarUsuario(const std::string& nombre, 
                                    const std::string& password,
                                    const std::string& tipo) {
    // Verificar si el usuario ya existe
    for (const auto& u : usuarios) {
        if (u->getNombre() == nombre) return false;
    }
    
    if (tipo == "profesor") {
        usuarios.push_back(std::make_shared<Profesor>(nombre, password));
    } else {
        usuarios.push_back(std::make_shared<Estudiante>(nombre, password));
    }
    return true;
}

//retorna un shared_ptr del usuario si esta registrado.
*/