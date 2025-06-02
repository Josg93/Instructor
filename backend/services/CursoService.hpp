#pragma once
#include "Curso.hpp"
#include "FileDatabase.hpp"
#include <vector>
#include <optional>

class CursoService {
private:
   
   
    
public:
    static std::vector<std::shared_ptr<Curso>> cursos;
    static std::vector<std::tuple<int, int, float, float, float, float>> notasCache;
    static std::vector<std::tuple<int, int>> inscripcionesCache;

    static void init();
    
    static void loadCursos();
    static void saveCursos();
    static void loadNotas();
    static void saveNotas();
    static void saveInscripciones();
    static void loadInscripciones();

    static std::shared_ptr<Curso> crearCurso(const std::string& nombre, int profesorId);
    static bool eliminarCurso(int cursoId);
    
    static bool inscribirEstudiante(int cursoId, int estudianteId);
    static bool expulsarEstudiante(int cursoId, int estudianteId);
    static bool desinscribirEstudiante(int estudianteId, int cursoId);
    
    static std::vector<std::shared_ptr<Curso>> listarCursos();
    static std::vector<std::shared_ptr<Curso>> listarCursosPorProfesor(int profesorId);
    static std::vector<std::shared_ptr<Curso>> listarCursosPorEstudiante(int estudianteId);
    static std::vector<std::pair<int, std::string>> listarEstudiantesInscritos(int cursoId);
    
    static bool asignarNota(int cursoId, int estudianteId,int numeroNota, float nota);
    static std::optional<std::tuple<float, float, float, float>> obtenerNotas(int cursoId, int estudianteId);
    
    static std::shared_ptr<Curso> obtenerCursoPorId(int id);
    static bool estaInscrito(int usuarioId , int cursoId);

    static bool desasignarProfesor(int cursoId);
    static bool asignarProfesor(int cursoId, int nuevoProfesorId);
};
