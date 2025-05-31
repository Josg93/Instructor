#include "Profesor.hpp"

Profesor::Profesor(const std::string& nombre,const std::string& email ,  const std::string& password)
    : Usuario(nombre,email,password) {}

std::string Profesor::getTipo() const { return "profesor"; }

void Profesor::agregarCurso(const std::shared_ptr<Curso>& curso) {
    cursosImpartidos.push_back(curso);
}

const std::vector<std::shared_ptr<Curso>>& Profesor::getCursosImpartidos() const {
    return cursosImpartidos;
}

bool Profesor::puedeModificarCurso(int cursoId) const {
    for (const auto& curso : cursosImpartidos) {
        if (curso->getId() == cursoId) {
            return true;
        }
    }
    return false;
}

