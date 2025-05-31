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




// Explicit template instantiations
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