// Profesor.hpp
#pragma once
#include "Usuario.hpp"
#include "Curso.hpp"
#include <memory>
#include <vector>

class Profesor : public Usuario {
private:
    std::vector<std::shared_ptr<Curso>> cursosImpartidos;

public:
    Profesor(const std::string& nombre,const std::string& email ,const std::string& password);
    ~Profesor() override = default;

    std::string getTipo() const override;
    void agregarCurso(const std::shared_ptr<Curso>& curso);
    const std::vector<std::shared_ptr<Curso>>& getCursosImpartidos() const;
    bool puedeModificarCurso(int cursoId) const;
};