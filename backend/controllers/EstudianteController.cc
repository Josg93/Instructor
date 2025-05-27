#include "EstudianteController.h"
#include "CursoService.hpp"
#include "ClaseService.hpp"
#include "../utils/JWT.h"
#include <cstdlib> // Para std::stoi

void EstudianteController::asyncHandleHttpRequest(const HttpRequestPtr& req,  std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        //La validacion del token devuelve el Id del usuario que esta haciendo solicitud
         auto Id = JWT::validarToken(req);

        if (!Id) {
            throw std::runtime_error("Token inválido o no proporcionado");
        }

        auto userId = Id.value();

        auto json = req->getJsonObject();

        //Inscribir estudiante        
        if (req->path() == "/estudiante/cursos" && req->method() == Post)
        {
            auto cursoId = json->get("curso_id", "0").asString();
            bool success = CursoService::inscribirEstudiante(std::stoi(cursoId), userId);
            
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Inscripción exitosa" : "Error al inscribirse";
            
            callback(HttpResponse::newHttpJsonResponse(respJson));
        }

    
            //Listar los cursos en los que está registrado el estudiante
        else if (req->path() == "/estudiante/cursos" && req->method() == Get)
         {
            auto cursos = CursoService::listarCursosPorEstudiante(userId);
            Json::Value array(Json::arrayValue);
            
            for (const auto& c : cursos) {
                Json::Value json;
                json["id"] = c->getId();
                json["nombre"] = c->getNombre();
                array.append(json);
            }
            
            callback(HttpResponse::newHttpJsonResponse(array));
        }

        //Obtener Notas del estudiante
        else if (req->path() == "/estudiante/notas" && req->method() == Get) 
        { 
            std::string cursoIdStr = req->getParameter("curso_id");
            if(cursoIdStr.empty()) cursoIdStr = "0";
            int cursoId = std::stoi(cursoIdStr);
            
            auto notas = CursoService::obtenerNotas(cursoId, userId);
            
            Json::Value response;
            if (notas) {
                response["status"] = "success";
                response["notas"] = Json::Value(Json::arrayValue);
                auto [n1, n2, n3, n4] = notas.value();
                response["notas"].append(n1);
                response["notas"].append(n2);
                response["notas"].append(n3);
                response["notas"].append(n4);
            } else {
                response["status"] = "error";
                response["message"] = "No se encontraron notas";
            }
            callback(HttpResponse::newHttpJsonResponse(response));
        }

        //
        /*
        else if (req->path() == "/estudiante/clases" && req->method() == Get) 
        {
         auto cursoIdStr = req->getParameter("curso_id");
            if(cursoIdStr.empty()) cursoIdStr = "0"; // asigna un valor por defecto para cursoIdStr
            
            int cursoId = std::stoi(cursoIdStr);
    
            // Verificar inscripción primero
            if (!CursoService::estaInscrito(userId, cursoId)) 
            {
                throw std::runtime_error("No estás inscrito en este curso");
            }   

        auto clases = ClaseService::listarClasesPorCurso(cursoId);
        Json::Value array(Json::arrayValue);
    
            for (const auto& c : clases)
             {
                Json::Value json;
                json["id"] = c->getId();
                json["titulo"] = c->getTitulo();
                json["url_youtube"] = c->getUrlYouTube();  // Nueva línea
                array.append(json);
            }
    
    callback(HttpResponse::newHttpJsonResponse(array));
            }
        */

        //ver clases:
        // Ver Clases
        else if (req->path() == "/estudiante/clases/ver" && req->method() == Get) 
        {
            auto claseIdStr = req->getParameter("clase_id");
            if(claseIdStr.empty()) {
                   throw std::runtime_error("Se requiere el ID de la clase");
            }
            int claseId = std::stoi(claseIdStr);
    
            auto clase = ClaseService::obtenerClasePorId(claseId);
            if (!clase) {
                throw std::runtime_error("Clase no encontrada");
            }

            // Verificar que el estudiante está inscrito en el curso
            bool tieneAcceso = CursoService::estaInscrito(userId, clase->getCursoId());
            if (!tieneAcceso) {
                throw std::runtime_error("No tienes acceso a esta clase");
            }

            Json::Value response;
            response["status"] = "success";
            response["data"] = Json::Value();
            response["data"]["id"] = clase->getId();
            response["data"]["titulo"] = clase->getTitulo();
            response["data"]["descripcion"] = clase->getContenido();
            response["data"]["url_youtube"] = clase->getUrlYouTube();
    
            callback(HttpResponse::newHttpJsonResponse(response));
        }


    } catch (const std::exception& e) {
        Json::Value errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        callback(HttpResponse::newHttpJsonResponse(errorResp));
    }
}