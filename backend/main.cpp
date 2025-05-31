#include <drogon/drogon.h>
//#include "filters/CorsFilter.h"
#include <filesystem>
#include <vector>
#include <fstream>

void initDataDirectory() {
    try {
        const std::string dataDir = "data";
        const std::vector<std::string> files = {
            "usuarios.txt", "cursos.txt", "clases.txt",
            "inscripciones.txt", "notas.txt"
        };

        // Crear directorio
        if (!std::filesystem::exists(dataDir)) {
            if (!std::filesystem::create_directory(dataDir)) {
                std::cerr << "Error: No se pudo crear el directorio 'data/'" << std::endl;
                exit(1);
            }
        }

        // Crear archivos
        for (const auto& file : files) {
            std::string path = dataDir + "/" + file;
            if (!std::filesystem::exists(path)) {
                std::ofstream out(path);
                if (!out) {
                    std::cerr << "Error: No se pudo crear " << path << std::endl;
                    continue;
                }
                out.close();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error crítico al inicializar datos: " << e.what() << std::endl;
        exit(1);
    }
}
int main() {

    initDataDirectory();
    
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 5555);
    std::cout << "Escuchando en el puerto 5555 alias puerto maldad" << std::endl;
    
    //drogon::app().registerFilter(std::make_shared<CorsFilter>(new CorsFilter()));

    

    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr& req, const drogon::HttpResponsePtr& resp)
        {
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "ContentType , Authorization");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
            resp->addHeader("Access-Control-Allow-Max-Age", "86400");
        }
    );
    
    //Load config file
    //drogon::app().loadConfigFile("../config.json");
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop

    std::cout << "Iniciando maldad: " << std::endl;
    drogon::app().run();
    return 0;
}
