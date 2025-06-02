#include "FileDatabase.hpp"
#include <fstream>
#include <sstream>
#include "Clase.hpp"
#include "Curso.hpp"
#include "Usuario.hpp"


std::vector<std::string> FileDatabase::readAll(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file("./data/" + filename);
    if (!file) return lines;
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

void FileDatabase::update(const std::string& filename, 
                        const std::vector<std::string>& lines) 
{
    std::ofstream file("./data/" + filename, std::ios::trunc);
    if (!file) throw std::runtime_error("Error al actualizar archivo: " + filename);
    for (const auto& line : lines) {
        file << line << "\n";
    }
}

template<typename T>
std::vector<T> FileDatabase::loadAll(const std::string& filename, 
                                   std::function<T(const std::string&)> parser)
{
    std::vector<T> items;
    auto lines = readAll(filename);
    for (const auto& line : lines) {
        items.push_back(parser(line));
    }
    return items;
}

template<typename T>
void FileDatabase::saveAll(const std::string& filename, 
                         const std::vector<T>& items,
                         std::function<std::string(const T&)> serializer) 
{
    std::vector<std::string> lines;
    for (const auto& item : items) {
        lines.push_back(serializer(item));
    }
    update(filename, lines);
}


std::vector<std::tuple<int, int, float, float, float, float>> 
FileDatabase::loadNotas(const std::string& filename) {
    auto lines = readAll(filename);
    std::vector<std::tuple<int, int, float, float, float, float>> notas;
    
    for (const auto& line : lines) {
        std::istringstream ss(line);
        std::string values[6];
        
        for (int i = 0; i < 6; ++i) {
            if (!std::getline(ss, values[i], ',')) {
                throw std::runtime_error("Formato de nota inválido");
            }
        }
        
        notas.emplace_back(
            std::stoi(values[0]), // curso_id
            std::stoi(values[1]), // estudiante_id
            std::stof(values[2]), // nota1
            std::stof(values[3]), // nota2
            std::stof(values[4]), // nota3
            std::stof(values[5])  // nota4
        );
    }
    
    return notas;
}

void FileDatabase::saveNotas(const std::string& filename,
                           const std::vector<std::tuple<int, int, float, float, float, float>>& notas) {
    std::vector<std::string> lines;
    for (const auto& nota : notas) {
        std::stringstream ss;
        ss << std::get<0>(nota) << ","
           << std::get<1>(nota) << ","
           << std::get<2>(nota) << ","
           << std::get<3>(nota) << ","
           << std::get<4>(nota) << ","
           << std::get<5>(nota);
        lines.push_back(ss.str());
    }
    update(filename, lines);
}




// Explicit template instantiations
// Añade estas líneas al final del archivo, después de las otras instanciaciones
template std::vector<std::tuple<int, int>> FileDatabase::loadAll<std::tuple<int, int>>(
    const std::string&, std::function<std::tuple<int, int>(const std::string&)>);

template void FileDatabase::saveAll<std::tuple<int, int>>(
    const std::string&, const std::vector<std::tuple<int, int>>&, 
    std::function<std::string(const std::tuple<int, int>&)>);

template std::vector<std::shared_ptr<Usuario>> FileDatabase::loadAll<std::shared_ptr<Usuario>>(
    const std::string&, std::function<std::shared_ptr<Usuario>(const std::string&)>);

template void FileDatabase::saveAll<std::shared_ptr<Usuario>>(
    const std::string&, const std::vector<std::shared_ptr<Usuario>>&, 
    std::function<std::string(const std::shared_ptr<Usuario>&)>);

template std::vector<std::shared_ptr<Curso>> FileDatabase::loadAll<std::shared_ptr<Curso>>(
    const std::string&, std::function<std::shared_ptr<Curso>(const std::string&)>);

template void FileDatabase::saveAll<std::shared_ptr<Curso>>(
    const std::string&, const std::vector<std::shared_ptr<Curso>>&, 
    std::function<std::string(const std::shared_ptr<Curso>&)>);

template std::vector<std::shared_ptr<Clase>> FileDatabase::loadAll<std::shared_ptr<Clase>>(
    const std::string&, std::function<std::shared_ptr<Clase>(const std::string&)>);

template void FileDatabase::saveAll<std::shared_ptr<Clase>>(
    const std::string&, const std::vector<std::shared_ptr<Clase>>&, 
    std::function<std::string(const std::shared_ptr<Clase>&)>);