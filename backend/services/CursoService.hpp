#pragma once
#include "Curso.hpp"
#include "FileDatabase.hpp"
#include <vector>
#include <optional>

class CursoService {
private:
    static std::vector<std::shared_ptr<Curso>> cursos;
    static void loadCursos();
    static void saveCursos();
    static void saveNotas();

public:
    static std::shared_ptr<Curso> crearCurso(const std::string& nombre, int profesorId);
    static bool inscribirEstudiante(int cursoId, int estudianteId);
    
    static std::vector<std::shared_ptr<Curso>> listarCursos();
    static std::vector<std::shared_ptr<Curso>> listarCursosPorProfesor(int profesorId);
    static std::vector<std::shared_ptr<Curso>> listarCursosPorEstudiante(int estudianteId);
    
    static bool asignarNota(int cursoId, int estudianteId, 
                          int numeroNota, float nota);
    static std::optional<std::tuple<float, float, float, float>> 
        obtenerNotas(int cursoId, int estudianteId);
    
    static std::shared_ptr<Curso> obtenerCursoPorId(int id);
    static bool estaInscrito(int usuarioId , int cursoId);
};

/*#pragma once
#include "Curso.hpp"
#include "Usuario.hpp"
#include <vector>
#include <memory>
#include <optional>

class CursoService {
private:
    static std::vector<std::shared_ptr<Curso>> cursos;

public:
    static std::shared_ptr<Curso> crearCurso(const std::string& nombre, int profesorId);
    static bool inscribirEstudiante(int cursoId, int estudianteId);
    
    static std::vector<std::shared_ptr<Curso>> listarCursos();
    static std::vector<std::shared_ptr<Curso>> listarCursosPorProfesor(int profesorId);
    static std::vector<std::shared_ptr<Curso>> listarCursosPorEstudiante(int estudianteId);
    
    static bool asignarNota(int cursoId, int estudianteId, 
                          int numeroNota, float nota);
    static std::optional<std::tuple<float, float, float, float>> 
        obtenerNotas(int cursoId, int estudianteId);
    
    static std::shared_ptr<Curso> obtenerCursoPorId(int id);
    static bool estaInscrito(int usuarioId , int cursoId);
};*/