#include "CursoService.hpp"
#include "ClaseService.hpp"
#include "FileDatabase.hpp"
#include "UsuarioService.hpp"
#include <algorithm>
#include <sstream>

std::vector<std::shared_ptr<Curso>> CursoService::cursos;
std::vector<std::tuple<int, int, float, float, float, float>> CursoService::notasCache;
std::vector<std::tuple<int, int>> CursoService::inscripcionesCache;

void CursoService::init() {
    loadCursos();
    loadNotas();
    loadInscripciones(); 
}

// Llamar al constructor estático (se ejecuta automáticamente)
namespace {
    struct StaticInit 
    {
        StaticInit() {
            CursoService::init();
        }
    };
    StaticInit staticInitializer;
}


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
bool CursoService::inscribirEstudiante(int cursoId, int estudianteId) 
{
    if(inscripcionesCache.empty())
    {
        loadInscripciones();
    }
    //hace dos cosas: inscribe al estudiante en el curso y 
    //inscribe el curso en la lista de cursos del estudiante
       if (auto curso = obtenerCursoPorId(cursoId)) 
       {
        auto it = std::find_if(inscripcionesCache.begin(), inscripcionesCache.end(),
            [cursoId, estudianteId](const auto& i) {
                return std::get<0>(i) == cursoId && std::get<1>(i) == estudianteId;
            });
        
        if (it != inscripcionesCache.end()) {
            return false; // Ya está inscrito
        }

        curso->inscribirEstudiante(estudianteId);

        inscripcionesCache.emplace_back(cursoId, estudianteId);
        saveInscripciones();       
        // Asociar curso al estudiante
        if (auto estudiante = std::dynamic_pointer_cast<Estudiante>(UsuarioService::obtenerPorId(estudianteId).value())) 
        {
            estudiante->inscribirEnCurso(curso);
        }
        return true;
    }
    return false;
}
//-----------------------------------------------------


bool CursoService::eliminarCurso(int cursoId) {
    // Eliminar inscripciones primero
    inscripcionesCache.erase(
        std::remove_if(inscripcionesCache.begin(), inscripcionesCache.end(),
            [cursoId](const auto& i) { return std::get<0>(i) == cursoId; }),
        inscripcionesCache.end()
    );
    saveInscripciones();
    
    // Resto de la lógica de eliminación...
    auto it = std::find_if(cursos.begin(), cursos.end(),
        [cursoId](const auto& c) { return c->getId() == cursoId; });
    
    if (it != cursos.end()) {
        // Eliminar clases asociadas
        auto clases = ClaseService::listarClasesPorCurso(cursoId);
        for (auto& clase : clases) {
            ClaseService::eliminarClase(clase->getId());
        }
        
        // Eliminar notas asociadas
        notasCache.erase(
            std::remove_if(notasCache.begin(), notasCache.end(),
                [cursoId](const auto& n) { return std::get<0>(n) == cursoId; }),
            notasCache.end()
        );
        saveNotas();
        
        cursos.erase(it);
        saveCursos();
        return true;
    }
    return false;
}


//-------------- Buscar un Curso por su Id -----------------------------
std::shared_ptr<Curso> CursoService::obtenerCursoPorId(int id) 
{
    auto it = std::find_if(cursos.begin(), cursos.end(), 
        [id](const auto& c) { return c->getId() == id; });
    return it != cursos.end() ? *it : nullptr;
}


//------------ Ver si un Estudiante está inscrito -------------
bool CursoService::estaInscrito(int usuarioId, int cursoId) 
{
    if(inscripcionesCache.empty())
    {
        loadInscripciones();
    }
    // Verificar en cache primero
    auto it = std::find_if(inscripcionesCache.begin(), inscripcionesCache.end(),
        [cursoId, usuarioId](const auto& i) {
            return std::get<0>(i) == cursoId && std::get<1>(i) == usuarioId;
        });
    
    if (it != inscripcionesCache.end()) {
        return true;
    }
    
    // Si no está en cache, verificar en el curso (para backward compatibility)
    if (auto curso = obtenerCursoPorId(cursoId)) {
        return curso->estaInscrito(usuarioId);
    }
    return false;
}


bool CursoService::expulsarEstudiante(int cursoId, int estudianteId) {
    // Eliminar de la cache de inscripciones
    auto inscrito = CursoService::estaInscrito(estudianteId,cursoId);
    if(!inscrito)
    {
        return false;
    }

    inscripcionesCache.erase(
        std::remove_if(inscripcionesCache.begin(), inscripcionesCache.end(),
            [cursoId, estudianteId](const auto& i) {
                return std::get<0>(i) == cursoId && std::get<1>(i) == estudianteId;
            }),
        inscripcionesCache.end()
    );
    saveInscripciones();
    
    // Eliminar del curso en memoria
    if (auto curso = obtenerCursoPorId(cursoId)) {
        auto& estudiantes = curso->getEstudiantes();
        auto it = std::remove_if(estudiantes.begin(), estudiantes.end(),
            [estudianteId](const auto& e) { return std::get<0>(e) == estudianteId; });
        
        if (it != estudiantes.end()) {
            estudiantes.erase(it, estudiantes.end());
            saveCursos();
        }
    }
    
    // Eliminar notas asociadas
    notasCache.erase(
        std::remove_if(notasCache.begin(), notasCache.end(),
            [cursoId, estudianteId](const auto& n) {
                return std::get<0>(n) == cursoId && std::get<1>(n) == estudianteId;
            }),
        notasCache.end()
    );
    saveNotas();
    
    return true;
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
    
    // Buscar en cache de inscripciones
    for (const auto& [cursoId, estId] : inscripcionesCache) {
        if (estId == estudianteId) {
            if (auto curso = obtenerCursoPorId(cursoId)) {
                resultado.push_back(curso);
            }
        }
    }
    
    return resultado;
}
std::vector<std::pair<int, std::string>> CursoService::listarEstudiantesInscritos(int cursoId) {
    std::vector<std::pair<int, std::string>> estudiantes;
    for (const auto& [curso_id, estId] : inscripcionesCache) {
        if (curso_id == cursoId) {
            if (auto usuario = UsuarioService::obtenerPorId(estId)) {
                estudiantes.emplace_back(estId, (*usuario)->getNombre());
            }
        }
    }
    return estudiantes;
}






//----------------- Metodo para que un profesor asigne una nota--------------
bool CursoService::asignarNota(int cursoId, int estudianteId,int numeroNota, float nota) 
{
    if(cursos.empty())
    {
        loadCursos();
    }

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

bool CursoService::desasignarProfesor(int cursoId) {
    auto curso = obtenerCursoPorId(cursoId);
    if (!curso) return false;
    
    // Valor especial para indicar "Sin profesor" (podría ser -1 o 0)
    
    saveCursos();
    return true;
}

bool CursoService::asignarProfesor(int cursoId, int nuevoProfesorId) {
    auto curso = obtenerCursoPorId(cursoId);
    if (!curso) return false;
    
    // Verificar que el nuevo usuario es realmente un profesor
    auto usuario = UsuarioService::obtenerPorId(nuevoProfesorId);
    if (!usuario || !std::dynamic_pointer_cast<Profesor>(*usuario)) {
        return false;
    }
    
    curso->setProfesorId(nuevoProfesorId);
    saveCursos();
    return true;
}


//----------- Retornar notas de un estudiante ------------------------
std::optional<std::tuple<float, float, float, float>> CursoService::obtenerNotas(int cursoId, int estudianteId)
{
    try {
        // Cargar datos si no están cargados
        if (cursos.empty()) loadCursos();
        if (notasCache.empty()) loadNotas();

        // Buscar en caché primero
        auto cacheIt = std::find_if(notasCache.begin(), notasCache.end(),
            [cursoId, estudianteId](const auto& n) {
                return std::get<0>(n) == cursoId && std::get<1>(n) == estudianteId;
            });

        if (cacheIt != notasCache.end()) {
            return std::make_tuple(
                std::get<2>(*cacheIt),
                std::get<3>(*cacheIt),
                std::get<4>(*cacheIt),
                std::get<5>(*cacheIt)
            );
        }

        // Si no está en caché, verificar en el curso
        auto curso = obtenerCursoPorId(cursoId);
        if (!curso) { return std::nullopt;}

        // Verificar inscripción
        if (!curso->estaInscrito(estudianteId)){return std::nullopt; }

        // Obtener notas (devolverá {0,0,0,0} si no hay)
        return curso->obtenerNotas(estudianteId);

    } catch (const std::exception& e) 
    {
        return std::nullopt;
    }
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


//---------------------NOTAS.TXT-------------
//Metodos especiales en FileDatabase pues los datos que guarda son particulares (tuplas) y se procesan diferente

void CursoService::loadNotas() {
    notasCache = FileDatabase::loadNotas("notas.txt");
}

void CursoService::saveNotas() {
    FileDatabase::saveNotas("notas.txt", notasCache);
}


//-------------------------------INSCRIPCIONES.TXT-----------------------------------
void CursoService::loadInscripciones() {
    auto parser = [](const std::string& line) -> std::tuple<int, int> {
        size_t pos = line.find(',');
        int cursoId = std::stoi(line.substr(0, pos));
        int estudianteId = std::stoi(line.substr(pos + 1));
        return std::make_tuple(cursoId, estudianteId);
    };
    
    inscripcionesCache = FileDatabase::loadAll<std::tuple<int, int>>("inscripciones.txt", parser);
}

void CursoService::saveInscripciones() {
    auto serializer = [](const std::tuple<int, int>& insc) -> std::string 
    {
        return std::to_string(std::get<0>(insc)) + "," + std::to_string(std::get<1>(insc));
    };
    
    FileDatabase::saveAll<std::tuple<int, int>>("inscripciones.txt", inscripcionesCache, serializer);
}
//-------------------------------------------------------------


