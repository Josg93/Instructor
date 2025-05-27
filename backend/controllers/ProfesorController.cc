#include "ProfesorController.h"
#include "CursoService.hpp"
#include "ClaseService.hpp"
#include "JWT.h"

void ProfesorController::asyncHandleHttpRequest(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback
) {
    try {
        // validacion de los tokens dl login
        auto Id = JWT::validarToken(req);

        if (!Id) {
            throw std::runtime_error("Token inválido o no proporcionado");
        }

        auto userId = Id.value();
        
        auto json = req->getJsonObject();
        
        //crear curso
        if (req->path() == "/profesor/cursos" && req->method() == Post) 
        {
            auto nombre = json->get("nombre", "").asString();
            auto curso = CursoService::crearCurso(nombre, userId);
            
            Json::Value respData;
            respData["status"] = "success";
            
            Json::Value dataObj;
            dataObj["id"] = curso->getId();
            dataObj["nombre"] = curso->getNombre();
            respData["data"] = dataObj;
            
            callback(HttpResponse::newHttpJsonResponse(respData));
        }

        //Listar cursos dados por el profesor
        else if (req->path() == "/profesor/cursos" && req->method() == Get) 
        {
            auto cursos = CursoService::listarCursosPorProfesor(userId);
            Json::Value array(Json::arrayValue);
            
            for (const auto& c : cursos) {
                Json::Value json;
                json["id"] = c->getId();
                json["nombre"] = c->getNombre();
                array.append(json);
            }
            
            callback(HttpResponse::newHttpJsonResponse(array));
        }

        //Asignar notas
        else if (req->path() == "/profesor/notas" && req->method() == Put) 
        {
            auto cursoId = json->get("curso_id", 0).asInt();
            auto estudianteId = json->get("estudiante_id", 0).asInt();
            auto nota = json->get("nota", 0.0).asFloat();
            auto numeroNota = json->get("numero_nota", 1).asInt();
            
            bool success = CursoService::asignarNota(cursoId, estudianteId, numeroNota, nota);
            
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Nota asignada" : "Error al asignar nota";
            
            callback(HttpResponse::newHttpJsonResponse(respJson));
        }


        //crear clase ---------------------------------------------------------
        else if (req->path() == "/profesor/clases" && req->method() == Post) 
        {   
            //esta linea maligna  verifica si existe json si curso_id es parte de json 
            //o si titulo e sparte de json
        if (!json || !json->isMember("curso_id") || !json->isMember("titulo"))
             {
                //verifica la maldad
                throw std::runtime_error("Datos incompletos en la solicitud");
             }

            // obtener cursoid como entero
            int curId;
         try 
        {
            curId = json->get("curso_id", "0").asInt();
        }
         catch (...) 
         {
            throw std::runtime_error("curso_id debe ser un número");
         }

        // Verifica que el cursoId sea válido (> 0)
        if (curId <= 0)
        {
            throw std::runtime_error("ID de curso inválido");
        }
        //validar si el profe es dueño del curso
        auto curso = CursoService::obtenerCursoPorId(curId);
        if (!curso || curso->getProfesorId() != userId) 
        {
            throw std::runtime_error("No tienes permiso para crear clases en este curso");
        }
        //si todas estas vlaidaciones no se ejecutan, ahora si viene la maldad:

            auto cursoId = json->get("curso_id" , "0").asInt();
            auto titulo = json->get("titulo", "").asString();
            auto urlYoutube = json->get("url_youtube","").asString();
            auto contenido = json->get("contenido", "").asString();
            
            auto clase = ClaseService::crearClase(titulo, contenido,urlYoutube, cursoId);
            
            //ajustar respuesta del controlador
            Json::Value respData;
            respData["status"] = "success";
            

            Json::Value dataObj;
            dataObj["id"] = clase->getId();
            dataObj["titulo"] = clase->getTitulo();
            respData["data"] = dataObj;
            
            callback(HttpResponse::newHttpJsonResponse(respData));
        }
    } catch (const std::exception& e) {
        Json::Value errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        callback(HttpResponse::newHttpJsonResponse(errorResp));
    }
}