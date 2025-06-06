#include "UsuarioService.hpp"
#include "CursoService.hpp"
#include "Estudiante.hpp"
#include "Profesor.hpp"
#include "Utils.hpp"
#include <sstream>

std::vector<std::shared_ptr<Usuario>> UsuarioService::usuarios;

//---------------- Registrar Usuarios Malignos ---------------
bool UsuarioService::registrarUsuario(const std::string& nombre,
                                      const std::string& email,
                                      const std::string& password,
                                      const std::string& tipo) 
{
    if (usuarios.empty()) 
        loadUsuarios();
    
    for (const auto& u : usuarios) 
    {
        if (u->getEmail() == email) return false;
    }

    if(nombre == "" || email == "")
    {
        return false;
    }
    if (tipo != "estudiante" && tipo != "profesor") {
        return false;
    }
    
    std::shared_ptr<Usuario> nuevo;
    std::string hashedPassword = Utils::hashPassword(password);
    if (tipo == "profesor")
    {
        nuevo = std::make_shared<Profesor>(nombre,email,hashedPassword);
    } 
    else 
    {
        nuevo = std::make_shared<Estudiante>(nombre,email,hashedPassword);
    }
    
    usuarios.push_back(nuevo);
    saveUsuarios();
    return true;
}





bool UsuarioService::eliminarUsuario(int usuarioId) {
    // 1. Buscar el usuario antes de eliminarlo
    auto usuario = obtenerPorId(usuarioId);
    if (!usuario) return false;

    // 2. Si es profesor, eliminar sus cursos (esto borra clases, notas e inscripciones)
    if (auto profesor = std::dynamic_pointer_cast<Profesor>(*usuario)) {
        auto cursos = CursoService::listarCursosPorProfesor(usuarioId);
        for (auto& curso : cursos) {
            CursoService::eliminarCurso(curso->getId());
        }
    }
    // 3. Si es estudiante, eliminar sus notas e inscripciones
    else if (auto estudiante = std::dynamic_pointer_cast<Estudiante>(*usuario)) {
        // Eliminar notas del estudiante en todos los cursos
        auto notas = CursoService::notasCache; // Acceder a la caché de notas
        notas.erase(
            std::remove_if(notas.begin(), notas.end(),
                [usuarioId](const auto& n) { return std::get<1>(n) == usuarioId; }),
            notas.end()
        );
        CursoService::saveNotas(); // Guardar cambios en notas.txt

        // Eliminar inscripciones del estudiante
        auto& inscripciones = CursoService::inscripcionesCache;
        inscripciones.erase(
            std::remove_if(inscripciones.begin(), inscripciones.end(),
                [usuarioId](const auto& i) { return std::get<1>(i) == usuarioId; }),
            inscripciones.end()
        );
        CursoService::saveInscripciones(); // Guardar cambios en inscripciones.txt
    }

    // 4. Eliminar al usuario de la lista y guardar cambios
    usuarios.erase(
        std::remove_if(usuarios.begin(), usuarios.end(),
            [usuarioId](const auto& u) { return u->getId() == usuarioId; }),
        usuarios.end()
    );
    saveUsuarios(); // Actualizar usuarios.txt

    return true;
}





bool UsuarioService::actualizarUsuario(int usuarioId,
                                        std::optional<std::string> nuevoNombre,
                                        std::optional<std::string> nuevoEmail,
                                        std::optional<std::string> nuevaPassword)
{
    auto usuario = obtenerPorId(usuarioId);
    if (!usuario) return false;
    
    if (nuevoNombre) {
        (*usuario)->setNombre(*nuevoNombre);
    }
    
    if(nuevoEmail)
    {
        (*usuario)->setEmail(*nuevoEmail);
    }

    if (nuevaPassword) {
        (*usuario)->setPassword(*nuevaPassword);
    }
    
    saveUsuarios();
    return true;
}





//funcion de autenticacion, VERIFICA QUE EL USUARIO ESTÉ INSCRITO
std::optional<std::shared_ptr<Usuario>> UsuarioService::autenticar(const std::string& nombre,
                                                                    const std::string& email, 
                                                                    const std::string& password) 
{
    if(usuarios.empty())
    {
        loadUsuarios();
    }

    auto it = std::find_if(usuarios.begin(), usuarios.end(), [&](const auto& u) { 
        return u->getNombre() == nombre && u->getEmail() == email , Utils::verifyPassword(password, u->getPassword());
    });
    return it != usuarios.end() ? std::make_optional(*it) : std::nullopt;
}


std::vector<std::shared_ptr<Usuario>> UsuarioService::obtenerUsuarios() {
    if (usuarios.empty()) {
        loadUsuarios();
    }
    return usuarios;
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

std::optional<std::shared_ptr<Usuario>> UsuarioService::obtenerPorEmail(std::string& email)
{
    if(UsuarioService::usuarios.empty())
    {
        return std::nullopt;
    }

    auto it = std::find_if(usuarios.begin() , usuarios.end() , [email](const auto& u){ return u->getEmail() == email;});

    return it != usuarios.end() ? std::make_optional(*it) : std::nullopt;
} 




//Listar techaers
std::vector<std::shared_ptr<Profesor>> UsuarioService::listarProfesores() {
    std::vector<std::shared_ptr<Profesor>> profesores;
    for (const auto& u : usuarios) {
        if (auto p = std::dynamic_pointer_cast<Profesor>(u)) {
            profesores.push_back(p);
        }
    }
    return profesores;
}



//------------------METODOS PARA GUARDAR EN TXT --------------------------------
void UsuarioService::loadUsuarios() {
    auto parser = [](const std::string& line) -> std::shared_ptr<Usuario> {
        std::istringstream ss(line);
        std::string tipo, nombre, email, password;
        std::getline(ss, tipo, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, email, ',');
        std::getline(ss, password);
        
        if (tipo == "profesor") {
            return std::make_shared<Profesor>(nombre, email, password);
        } else {
            return std::make_shared<Estudiante>(nombre, email ,password);
        }
    };
    
    usuarios = FileDatabase::loadAll<std::shared_ptr<Usuario>>("usuarios.txt", parser);
}

void UsuarioService::saveUsuarios() {
    auto serializer = [](const std::shared_ptr<Usuario>& u) -> std::string {
        return u->getTipo() + "," + u->getNombre() + "," + u->getEmail() + "," + u->getPassword();
    };
    
    FileDatabase::saveAll<std::shared_ptr<Usuario>>("usuarios.txt", usuarios, serializer);
}
//-------------------------------------------------------------------------------------------




/*#include "UsuarioService.hpp"
#include <algorithm>

std::vector<std::shared_ptr<Usuario>> UsuarioService::usuarios;




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