#pragma once
#include <string>
#include <memory>

class Clase {
private:
    int id;
    std::string titulo;
    std::string contenido;
    std::string urlYoutube;
    int cursoId;
    static int ultimoId;

public:
    Clase(const std::string& titulo, const std::string& contenido,const std::string& url , int cursoId);
    
    // Getters
    int getId() const;
    std::string getTitulo() const;
    std::string getContenido() const;
    std::string getUrlYouTube() const;
    int getCursoId() const;

    void setId(int id_){ this->id = id_;}
};