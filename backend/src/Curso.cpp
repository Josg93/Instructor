#include "Curso.hpp"
#include "Clase.hpp"

int Curso::ultimoId = 0;

Curso::Curso(const std::string& nombre, int profesorId)
    : id(++ultimoId), nombre(nombre), profesorId(profesorId) {}


//----------------- getters --------------------
int Curso::getId() const { return id; }
std::string Curso::getNombre() const { return nombre; }
int Curso::getProfesorId() const { return profesorId; }


// -------------- Inscribir estudiante en el curso 
void Curso::inscribirEstudiante(int estudianteId) {
    estudiantes.emplace_back(estudianteId, 0.0f, 0.0f, 0.0f, 0.0f);
}

// ---------------------- Verificar si el estudiante esta inscrito 
bool Curso::estaInscrito(int estudianteId) const {
    for (const auto& est : estudiantes) {
        if (std::get<0>(est) == estudianteId) {
            return true;
        }
    }
    return false;
}


//-----------  asignar nota a un estudiante 
void Curso::asignarNota(int estudianteId, int numeroNota, float nota) {
    // Validación de nota...
    for (auto& est : estudiantes) {
        if (std::get<0>(est) == estudianteId) {
            switch (numeroNota) {
                case 1: std::get<1>(est) = nota; break;
                case 2: std::get<2>(est) = nota; break;
                case 3: std::get<3>(est) = nota; break;
                case 4: std::get<4>(est) = nota; break;
                default: throw std::invalid_argument("Número de nota inválido");
            }
            return;
        }
    }
    throw std::invalid_argument("Estudiante no inscrito");
}


// ------------------------ Obtener notas de un estudiante -------------
std::tuple<float, float, float, float> Curso::obtenerNotas(int estudianteId) const 
{
    for (const auto& est : estudiantes) {
        if (std::get<0>(est) == estudianteId) {
            return {std::get<1>(est), std::get<2>(est), std::get<3>(est), std::get<4>(est)};
        }
    }
    throw std::invalid_argument("Estudiante no inscrito");
}


// ------------- agregar clase ------------------------
void Curso::agregarClase(const std::shared_ptr<Clase>& clase)
{
    clases.push_back(clase);
}

const std::vector<std::shared_ptr<Clase>>& Curso::getClases() const
{
    return clases;
}

