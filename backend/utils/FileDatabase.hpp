#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <tuple>

class FileDatabase {
public:
    // Métodos genéricos (para Usuario, Curso, Clase)
    template<typename T>
    static std::vector<T> loadAll(const std::string& filename, 
                                std::function<T(const std::string&)> parser);
    
    template<typename T>
    static void saveAll(const std::string& filename, 
                      const std::vector<T>& items,
                      std::function<std::string(const T&)> serializer);
    
    // Métodos específicos para notas
    static std::vector<std::tuple<int, int, float, float, float, float>> loadNotas(const std::string& filename);
    static void saveNotas(const std::string& filename, 
                        const std::vector<std::tuple<int, int, float, float, float, float>>& notas);
    
    // Métodos auxiliares
    static std::vector<std::string> readAll(const std::string& filename);
    static void update(const std::string& filename, 
                     const std::vector<std::string>& lines);
};