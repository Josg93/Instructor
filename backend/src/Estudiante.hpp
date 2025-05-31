#pragma once
#include "Usuario.hpp"
#include "Curso.hpp"
#include <memory>
#include <vector>

class Estudiante : public Usuario {
private:
    std::vector<std::weak_ptr<Curso>> cursosInscritos;

public:
    Estudiante(const std::string& nombre,const std::string& email , const std::string& password);
    ~Estudiante() override = default;

    std::string getTipo() const override;
    void inscribirEnCurso(const std::shared_ptr<Curso>& curso);
    const std::vector<std::weak_ptr<Curso>>& getCursosInscritos() const;
};