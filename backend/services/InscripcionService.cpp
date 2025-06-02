#include "InscripcionService.hpp"
#include <algorithm>

std::vector<std::tuple<int, int>> InscripcionService::inscripciones;

// Inicialización estática

void InscripcionService::init() {
    loadInscripciones();
}
namespace {
    struct StaticInit {
        StaticInit() {
            InscripcionService::init();
        }
    };
    StaticInit staticInitializer;
}




bool InscripcionService::inscribirEstudiante(int cursoId, int estudianteId) {
    // Verificar si ya está inscrito
    auto it = std::find_if(inscripciones.begin(), inscripciones.end(),
        [cursoId, estudianteId](const auto& i) {
            return std::get<0>(i) == cursoId && std::get<1>(i) == estudianteId;
        });
    
    if (it == inscripciones.end()) {
        inscripciones.emplace_back(cursoId, estudianteId);
        saveInscripciones();
        return true;
    }
    return false;
}

bool InscripcionService::desinscribirEstudiante(int cursoId, int estudianteId) {
    auto it = std::remove_if(inscripciones.begin(), inscripciones.end(),
        [cursoId, estudianteId](const auto& i) {
            return std::get<0>(i) == cursoId && std::get<1>(i) == estudianteId;
        });
    
    if (it != inscripciones.end()) {
        inscripciones.erase(it, inscripciones.end());
        saveInscripciones();
        return true;
    }
    return false;
}

bool InscripcionService::estaInscrito(int estudianteId, int cursoId) {
    auto it = std::find_if(inscripciones.begin(), inscripciones.end(),
        [cursoId, estudianteId](const auto& i) {
            return std::get<0>(i) == cursoId && std::get<1>(i) == estudianteId;
        });
    return it != inscripciones.end();
}

std::vector<int> InscripcionService::obtenerCursosDeEstudiante(int estudianteId) {
    std::vector<int> cursos;
    for (const auto& [cursoId, estId] : inscripciones) {
        if (estId == estudianteId) {
            cursos.push_back(cursoId);
        }
    }
    return cursos;
}

std::vector<int> InscripcionService::obtenerEstudiantesDeCurso(int cursoId) {
    std::vector<int> estudiantes;
    for (const auto& [cId, estudianteId] : inscripciones) {
        if (cId == cursoId) {
            estudiantes.push_back(estudianteId);
        }
    }
    return estudiantes;
}

void InscripcionService::eliminarInscripcionesDeCurso(int cursoId) {
    inscripciones.erase(
        std::remove_if(inscripciones.begin(), inscripciones.end(),
            [cursoId](const auto& i) { return std::get<0>(i) == cursoId; }),
        inscripciones.end()
    );
    saveInscripciones();
}


void InscripcionService::loadInscripciones() {
    auto parser = [](const std::string& line) -> std::tuple<int, int> {
        size_t pos = line.find(',');
        int cursoId = std::stoi(line.substr(0, pos));
        int estudianteId = std::stoi(line.substr(pos + 1));
        return std::make_tuple(cursoId, estudianteId);
    };
    
    inscripciones = FileDatabase::loadAll<std::tuple<int, int>>("inscripciones.txt", parser);
}

void InscripcionService::saveInscripciones() {
    auto serializer = [](const std::tuple<int, int>& insc) -> std::string {
        return std::to_string(std::get<0>(insc)) + "," + std::to_string(std::get<1>(insc));
    };
    
    FileDatabase::saveAll<std::tuple<int, int>>("inscripciones.txt", inscripciones, serializer);
}