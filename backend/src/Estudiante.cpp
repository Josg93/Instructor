#include "Estudiante.hpp"

Estudiante::Estudiante(const std::string& nombre, const std::string& email , const std::string& password)
    : Usuario(nombre,email,password) {}

std::string Estudiante::getTipo() const { return "estudiante"; }

void Estudiante::inscribirEnCurso(const std::shared_ptr<Curso>& curso) {
    cursosInscritos.push_back(curso);
}

const std::vector<std::weak_ptr<Curso>>& Estudiante::getCursosInscritos() const {
    return cursosInscritos;
}

