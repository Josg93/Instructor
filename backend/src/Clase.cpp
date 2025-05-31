#include "Clase.hpp"

int Clase::ultimoId = 0;

Clase::Clase(const std::string& titulo,
             const std::string& contenido,
             const std::string& url, 
             int cursoId)
    : id(++ultimoId), titulo(titulo), contenido(contenido), urlYoutube(url), cursoId(cursoId) {}

int Clase::getId() const { return id; }
std::string Clase::getTitulo() const { return titulo; }
std::string Clase::getContenido() const { return contenido; }
std::string Clase::getUrlYouTube() const {return urlYoutube;}
int Clase::getCursoId() const { return cursoId; }

void Clase::setUrlYouTube(const std::string& nuevaUrl){urlYoutube = nuevaUrl;}