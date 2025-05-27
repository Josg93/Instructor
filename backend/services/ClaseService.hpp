#pragma once
#include "Clase.hpp"
#include "FileDatabase.hpp"
#include <vector>

class ClaseService {
private:
    static std::vector<std::shared_ptr<Clase>> clases;
    static void loadClases();
    static void saveClases();

public:
    static std::shared_ptr<Clase> crearClase(const std::string& titulo, const std::string& contenido, const std::string& url, int cursoId);
    static std::vector<std::shared_ptr<Clase>> listarClasesPorCurso(int cursoId);
    static std::shared_ptr<Clase> obtenerClasePorId(int id);
};


/*#pragma once
#include "Clase.hpp"
#include "Curso.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class ClaseService {
private:
    static std::vector<std::shared_ptr<Clase>> clases;

public:
    static std::shared_ptr<Clase> crearClase(const std::string& titulo, 
                                           const std::string& contenido,
                                           const std::string& urlYoutube,
                                           int cursoId);
    
    static std::vector<std::shared_ptr<Clase>> listarClasesPorCurso(int cursoId);
    static std::shared_ptr<Clase> obtenerClasePorId(int id);
   
};
*/