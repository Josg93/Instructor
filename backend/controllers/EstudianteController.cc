#include "EstudianteController.h"
#include "CursoService.hpp"
#include "ClaseService.hpp"
#include "UsuarioService.hpp"
#include "Utils.hpp"
#include "../utils/JWT.h"
#include <cstdlib> 


void EstudianteController::asyncHandleHttpRequest(const HttpRequestPtr& req,  std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
      
        // validacion de Requests y tokens malignos -------------------------------
        auto Id = JWT::validarToken(req);
        if (!Id) {throw std::runtime_error("Token inválido o no proporcionado");}
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

            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            if(!success)
            {
                resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
            }
            
            callback(resp);
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

        //VER NOTAS Obtener Notas del estudiante
        else if (req->path() == "/estudiante/notas" && req->method() == Get) 
{ 
        try 
        {
            // Obtener parámetro
            auto cursoIdStr = req->getParameter("curso_id");
            if(cursoIdStr.empty()) {
                throw std::runtime_error("Se requiere el parámetro curso_id");
            }
            int cursoId = std::stoi(cursoIdStr);

            // Verificar inscripción primero
            if(!CursoService::estaInscrito(userId, cursoId)) {
                throw std::runtime_error("No estás inscrito en este curso");
            }

            // Obtener notas
            auto notas = CursoService::obtenerNotas(cursoId, userId);

            Json::Value response;
            if (notas) {
                response["status"] = "success";
                auto [n1, n2, n3, n4] = notas.value();
                response["notas"] = Json::objectValue;
                response["notas"]["n1"] = n1;
                response["notas"]["n2"] = n2;
                response["notas"]["n3"] = n3;
                response["notas"]["n4"] = n4;
            } else {
                // Si no hay notas registradas, devolver ceros
                response["status"] = "success";
                response["notas"] = Json::objectValue;
                response["notas"]["n1"] = 0.0;
                response["notas"]["n2"] = 0.0;
                response["notas"]["n3"] = 0.0;
                response["notas"]["n4"] = 0.0;
            }

             callback(HttpResponse::newHttpJsonResponse(response));

        } catch (const std::exception& e) {
            Json::Value error;
            error["status"] = "error";
            error["message"] = e.what();
            auto resp = HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
            callback(resp);
        }
}

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


        // Desinscribirse de curso 
        else if(req->path() == "/estudiante/cursos/salir" && req->method() == Delete) 
        {
            auto cursoId = json->get("curso_id", 0).asInt();

            bool inscrito = CursoService::estaInscrito(userId, cursoId);
         if(!inscrito) 
         {
            throw std::runtime_error("Usted no se encuentra inscrito en este curso");
         }

             bool success = CursoService::desinscribirEstudiante(userId, cursoId);

             Json::Value respJson;
             respJson["status"] = success ? "success" : "error";
             respJson["message"] = success ? "Desinscrito del curso" : "Error al desinscribirse";

             auto resp = HttpResponse::newHttpJsonResponse(respJson);
             resp->setStatusCode(success ? k200OK : k500InternalServerError);
             callback(resp);
        }


        // Actualizar perfil (nombre y contraseña)
        else if (req->path() == "/estudiante/perfil" && req->method() == Put) 
        {
            auto nuevoNombre = json->get("nombre", "").asString();
            auto nuevoEmail = json->get("email", "").asString();
            auto nuevaPassword = json->get("password", "").asString();

            if (nuevoNombre.empty() && nuevoEmail.empty() && nuevaPassword.empty()) {
                throw std::runtime_error("No hay datos para actualizar");
            }

            auto usuario = UsuarioService::obtenerPorId(userId);
            if (!usuario) {
                throw std::runtime_error("Usuario no encontrado");
            }

            bool success = UsuarioService::actualizarUsuario(
                userId,
                nuevoNombre.empty() ? std::nullopt : std::optional<std::string>(nuevoNombre),
                nuevoEmail.empty() ? std::nullopt : std::optional<std::string>(nuevoEmail),
                nuevaPassword.empty() ? std::nullopt : std::optional<std::string>(Utils::hashPassword(nuevaPassword))
            );

            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Perfil actualizado" : "Error al actualizar perfil";

            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
        }

    } catch (const std::exception& e) {
        Json::Value errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        callback(HttpResponse::newHttpJsonResponse(errorResp));
    }


}