#include "CursoService.hpp"
#include "UsuarioService.hpp"
#include <algorithm>
#include <sstream>

std::vector<std::shared_ptr<Curso>> CursoService::cursos;
static std::vector<std::tuple<int, int, float, float, float, float>> notasCache;



//----- METODO MALIGNO DE CREAR CURSO -----------------------------------------------
std::shared_ptr<Curso> CursoService::crearCurso(const std::string& nombre, int profesorId) 
{
    //crear el  curso
    auto curso = std::make_shared<Curso>(nombre, profesorId);
    cursos.push_back(curso);
    saveCursos();
    
    // Asociar curso al profesor
    auto usuario = UsuarioService::obtenerPorId(profesorId);
        if (!usuario) {
            cursos.pop_back();  // Revertir la creación del curso
            throw std::runtime_error("Usuario no encontrado");
        }

        auto profesor = std::dynamic_pointer_cast<Profesor>(*usuario);
        if (!profesor) {
            cursos.pop_back();  // Revertir la creación del curso
            throw std::runtime_error("El usuario no es un profesor");
        }

        
        profesor->agregarCurso(curso);
    
    return curso;
}


//--------- Inscribir Estudiante al Curso -----------------------------
bool CursoService::inscribirEstudiante(int cursoId, int estudianteId) {
    //hace dos cosas: inscribe al estudiante en el curso y 
    //inscribe el curso en la lista de cursos del estudiante
    if (auto curso = obtenerCursoPorId(cursoId)) {
        curso->inscribirEstudiante(estudianteId);
        
        // Asociar curso al estudiante
        if (auto estudiante = std::dynamic_pointer_cast<Estudiante>(
            UsuarioService::obtenerPorId(estudianteId).value())) 
        {
            estudiante->inscribirEnCurso(curso);
        }
        return true;
    }
    return false;
}
//-----------------------------------------------------
bool CursoService::eliminarCurso(int cursoId) 
{
    auto it = std::find_if(cursos.begin(), cursos.end(), 
        [cursoId](const auto& c) { return c->getId() == cursoId; });
    
    if (it != cursos.end()) {
        cursos.erase(it);
        saveCursos();
        return true;
    }
    return false;
}

bool CursoService::expulsarEstudiante(int cursoId, int estudianteId) 
{
    auto curso = obtenerCursoPorId(cursoId);
    if (!curso) return false;
    
    // Implementar lógica para expulsar estudiante
    auto& estudiantes = curso->getEstudiantes();
    auto it = std::remove_if(estudiantes.begin(), estudiantes.end(),
        [estudianteId](const auto& e) { return std::get<0>(e) == estudianteId; });
    
    if (it != estudiantes.end()) {
        estudiantes.erase(it, estudiantes.end());
        saveCursos();
        return true;
    }
    return false;
}

bool CursoService::desinscribirEstudiante(int estudianteId, int cursoId)
{
    // Similar a expulsar pero iniciado por el estudiante
    return expulsarEstudiante(cursoId, estudianteId);
}


//------------------ Listar todos los cursos que existen ----------------------------
std::vector<std::shared_ptr<Curso>> CursoService::listarCursos() 
{
    return cursos;
}


//------------------------- Cursos Dados de un profeso
std::vector<std::shared_ptr<Curso>> CursoService::listarCursosPorProfesor(int profesorId) 
{
    std::vector<std::shared_ptr<Curso>> resultado;
    for (const auto& c : cursos) {
        if (c->getProfesorId() == profesorId) {
            resultado.push_back(c);
        }
    }
    return resultado;
}


// ------------------------- Cursos en los que esta un estudiante
std::vector<std::shared_ptr<Curso>> CursoService::listarCursosPorEstudiante(int estudianteId) 
{
    std::vector<std::shared_ptr<Curso>> resultado;
    for (const auto& c : cursos) {
        if (c->estaInscrito(estudianteId)) {
            resultado.push_back(c);
        }
    }
    return resultado;
}


//----------------- Metodo para que un profesor asigne una nota--------------
bool CursoService::asignarNota(int cursoId, int estudianteId,int numeroNota, float nota) 
{
    if (auto curso = obtenerCursoPorId(cursoId)) {
        try {
            if(numeroNota < 1 || numeroNota > 4)
            {
                throw std::runtime_error("La nota no es valida");
            }
            curso->asignarNota(estudianteId, numeroNota, nota);

            //it en este caso me va a retornar una dupla CursoId, EstudianteId
            auto it = std::find_if(notasCache.begin(), notasCache.end(),
                [&](const auto& n) {
                    return std::get<0>(n) == cursoId && std::get<1>(n) == estudianteId;
                });
            
            //Aqui se toma it y se toma el valor 2,3,4,5 de la tupla para modificarlo en funcion
            if (it != notasCache.end()) {
                // Actualizar nota existente
                switch(numeroNota) {
                    case 1: std::get<2>(*it) = nota; break;
                    case 2: std::get<3>(*it) = nota; break;
                    case 3: std::get<4>(*it) = nota; break;
                    case 4: std::get<5>(*it) = nota; break;
                }
            //WTF
            } else {notasCache.emplace_back(cursoId, estudianteId, 
                    numeroNota == 1 ? nota : 0.0f,
                    numeroNota == 2 ? nota : 0.0f,
                    numeroNota == 3 ? nota : 0.0f,
                    numeroNota == 4 ? nota : 0.0f);
            }
            
            saveNotas();
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}


//----------- Retornar notas de un estudiante ------------------------
std::optional<std::tuple<float, float, float, float>> CursoService::obtenerNotas(int cursoId,
                                                                                 int estudianteId)
{
    if (auto curso = obtenerCursoPorId(cursoId)) {
        try {
            return curso->obtenerNotas(estudianteId);
        } catch (...) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}


//-------------- Buscar un Curso por su Id -----------------------------
std::shared_ptr<Curso> CursoService::obtenerCursoPorId(int id) 
{
    auto it = std::find_if(cursos.begin(), cursos.end(), 
        [id](const auto& c) { return c->getId() == id; });
    return it != cursos.end() ? *it : nullptr;
}


//------------ Ver si un Estudiante está inscrito -------------
bool CursoService::estaInscrito(int usuarioId , int cursoId)
{
    auto curso = CursoService::obtenerCursoPorId(cursoId);
    if(curso->estaInscrito(usuarioId))
    {
        return true;
    }
    return false;
}


//---------- Guardar cursos en txt -----------------
void CursoService::loadCursos()
{
    auto parser = [](const std::string& line) -> std::shared_ptr<Curso> {
        std::istringstream ss(line);
        std::string sid, nombre, sprofesor;
        std::getline(ss, sid, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, sprofesor);
        
        auto curso = std::make_shared<Curso>(nombre, std::stoi(sprofesor));
        curso->setId(std::stoi(sid));
        return curso;
    };
    
    cursos = FileDatabase::loadAll<std::shared_ptr<Curso>>("cursos.txt", parser);
}

void CursoService::saveCursos()
{
    auto serializer = [](const std::shared_ptr<Curso>& c) -> std::string {
        return std::to_string(c->getId()) + "," + c->getNombre() + "," + std::to_string(c->getProfesorId());
    };
    
    FileDatabase::saveAll<std::shared_ptr<Curso>>("cursos.txt", cursos, serializer);
}

void CursoService::saveNotas() 
{
    std::vector<std::string> lines;
    for (const auto& nota : notasCache) {
        std::stringstream ss;
        ss << std::get<0>(nota) << ","  // curso_id
           << std::get<1>(nota) << ","  // estudiante_id
           << std::get<2>(nota) << ","  // nota1
           << std::get<3>(nota) << ","  // nota2
           << std::get<4>(nota) << ","  // nota3
           << std::get<5>(nota);        // nota4
        lines.push_back(ss.str());
    }
    FileDatabase::update("notas.txt", lines);
}
//-------------------------------------------------------------


