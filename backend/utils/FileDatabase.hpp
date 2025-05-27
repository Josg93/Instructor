#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>

class FileDatabase {
public:
    template<typename T>
    static std::vector<T> loadAll(const std::string& filename, 
                                std::function<T(const std::string&)> parser);
    
    template<typename T>
    static void saveAll(const std::string& filename, 
                      const std::vector<T>& items,
                      std::function<std::string(const T&)> serializer);
    
    static std::vector<std::string> readAll(const std::string& filename);
    static void update(const std::string& filename, 
                     const std::vector<std::string>& lines);
};