
#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <stdexcept>

class Clase;

class Curso {
private:
    int id;
    std::string nombre;
    int profesorId;
    std::vector<std::tuple<int, float, float, float, float>> estudiantes; // id, nota1, nota2, nota3, nota4
    std::vector<std::shared_ptr<Clase>> clases;
    static int ultimoId;

public:
    Curso(const std::string& nombre, int profesorId);
    
    // Getters
    int getId() const;
    std::string getNombre() const;
    int getProfesorId() const;
    std::vector<std::tuple<int,float,float,float,float>>& getEstudiantes();

    // Estudiantes
    void inscribirEstudiante(int estudianteId);
    bool EliminarEstudiante(int estudianteId);
    bool estaInscrito(int estudianteId) const;
    
    // Notas
    void asignarNota(int estudianteId, int numeroNota, float nota);
    std::tuple<float, float, float, float> obtenerNotas(int estudianteId) const;
    
    // Clases
    void agregarClase(const std::shared_ptr<Clase>& clase);
    const std::vector<std::shared_ptr<Clase>>& getClases() const;
    
    void setId(int id_){ this->id = id_;}
    void setNombre(const std::string& nuevoNombre) { nombre = nuevoNombre; }
    void setProfesorId(int id){profesorId = id;}
};