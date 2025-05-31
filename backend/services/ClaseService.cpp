#include "ClaseService.hpp"
#include "CursoService.hpp"
#include <sstream>

std::vector<std::shared_ptr<Clase>> ClaseService::clases;

// ----------------- Crear Clase Nueva -------------------
std::shared_ptr<Clase> ClaseService::crearClase(const std::string& titulo, const std::string& contenido, const std::string& url, int cursoId) {
    if (clases.empty()) loadClases();
    
    // Resto de la lógica permanece igual
     auto curso = CursoService::obtenerCursoPorId(cursoId);
    if (!curso) {
        throw std::runtime_error("El curso no existe");
    }
    
    // llama al constructor de Clase y crea una clase maligna
    auto clase = std::make_shared<Clase>(titulo, contenido,url,cursoId);
    clases.push_back(clase);
    saveClases();
    
    curso->agregarClase(clase);
    
    return clase;
}



// ------------------------------Listar Clases Por curso ----------------------------
std::vector<std::shared_ptr<Clase>> ClaseService::listarClasesPorCurso(int cursoId) {
    std::vector<std::shared_ptr<Clase>> resultado;
    for (const auto& c : clases) {
        if (c->getCursoId() == cursoId) {
            resultado.push_back(c);
        }
    }
    return resultado;
}

bool ClaseService::actualizarUrlClase(int claseId, const std::string& nuevaUrl) {
    auto clase = obtenerClasePorId(claseId);
    if (clase) {
        clase->setUrlYouTube(nuevaUrl);
        saveClases();
        return true;
    }
    return false;
}

// -------------------------- Obtener Clases por Id ----------------------------
std::shared_ptr<Clase> ClaseService::obtenerClasePorId(int id) {
    auto it = std::find_if(clases.begin(), clases.end(), 
        [id](const auto& c) { return c->getId() == id; });
    return it != clases.end() ? *it : nullptr;
}

// ------------------ GUARDAR Y CARGAR ELEMENTOS EN UN TXT --------------------
void ClaseService::loadClases() {
    auto parser = [](const std::string& line) -> std::shared_ptr<Clase> {
        std::istringstream ss(line);
        std::string sid, scurso, titulo, contenido, url;
        std::getline(ss, sid, ',');
        std::getline(ss, scurso, ',');
        std::getline(ss, titulo, ',');
        std::getline(ss, contenido, ',');
        std::getline(ss, url);
        
        auto clase = std::make_shared<Clase>(titulo, contenido, url, std::stoi(scurso));
        clase->setId(std::stoi(sid));
        return clase;
    };
    
    clases = FileDatabase::loadAll<std::shared_ptr<Clase>>("clases.txt", parser);
}

void ClaseService::saveClases() {
    auto serializer = [](const std::shared_ptr<Clase>& c) -> std::string {
        return std::to_string(c->getId()) + "," + 
               std::to_string(c->getCursoId()) + "," +
               c->getTitulo() + "," +
               c->getContenido() + "," +
               c->getUrlYouTube();
    };
    
    FileDatabase::saveAll<std::shared_ptr<Clase>>("clases.txt", clases, serializer);
}
//----------------------------------------------------------------------------------

