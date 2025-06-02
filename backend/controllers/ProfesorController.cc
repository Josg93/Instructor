#include "ProfesorController.h"
#include "CursoService.hpp"
#include "ClaseService.hpp"
#include "UsuarioService.hpp"
#include "Utils.hpp"
#include "JWT.h"

void ProfesorController::asyncHandleHttpRequest(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback
) {
    try {
    
        // validacion de los tokens dl login
        auto Id = JWT::validarToken(req);
        if (!Id) {throw std::runtime_error("Token inválido o no proporcionado");}
        auto userId = Id.value();
        auto json = req->getJsonObject();


        // Actualizar perfil (nombre y contraseña)
        if (req->path() == "/profesor/perfil" && req->method() == Put) 
        {
            auto nuevoNombre = json->get("nombre", "").asString();
            auto nuevoEmail = json->get("email", "").asString();
            auto nuevaPassword = json->get("password", "").asString();
    
            if (nuevoNombre.empty() && nuevoEmail.empty() && nuevaPassword.empty()) 
            {
                throw std::runtime_error("No hay datos para actualizar");
            }
    
            auto usuario = UsuarioService::obtenerPorId(userId);
            if (!usuario) 
            {
                throw std::runtime_error("Usuario no encontrado");
            }
    
            bool success = UsuarioService::actualizarUsuario(
            userId,
            nuevoNombre.empty() ? std::nullopt : std::optional<std::string>(nuevoNombre),
            nuevoEmail.empty() ? std::nullopt : std::optional<std::string>(nuevoEmail),
            nuevaPassword.empty() ? std::nullopt : std::optional<std::string>(Utils::hashPassword(nuevaPassword)));
    
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Perfil actualizado" : "Error al actualizar perfil";
            
            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
        }


        
        //crear curso
        else if (req->path() == "/profesor/cursos" && req->method() == Post) 
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

        // Dejar de dar un curso
        else if (req->path() == "/profesor/cursos/abandonar" && req->method() == Post) {
            auto cursoId = json->get("curso_id", 0).asInt();
            
            // Verificar que el profesor es dueño del curso
            auto curso = CursoService::obtenerCursoPorId(cursoId);
            if (!curso || curso->getProfesorId() != userId) {
                throw std::runtime_error("No tienes permiso para esta acción");
            }
            
            bool success = CursoService::desasignarProfesor(cursoId);
            
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Has dejado de dar este curso" : "Error al abandonar curso";
            
            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
        }

    // Eliminar curso --------------------------------
        else if (req->path() == "/profesor/cursos" && req->method() == Delete) 
        {
            auto cursoId = json->get("curso_id", 0).asInt();
    
         // Verificar que el profesor es dueño del curso
            auto curso = CursoService::obtenerCursoPorId(cursoId);
            if (!curso || curso->getProfesorId() != userId) 
            {
                throw std::runtime_error("No tienes permiso para eliminar este curso");
            }
    
            bool success = CursoService::eliminarCurso(cursoId);
    
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Curso eliminado" : "Error al eliminar curso";
    
            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
        }

        // Expulsar estudiante ------------------------------------------------------------
        else if (req->path() == "/profesor/cursos/expulsar" && req->method() == Post)
        {
            auto cursoId = json->get("curso_id", 0).asInt();
            auto estudianteId = json->get("estudiante_id", 0).asInt();
    
             // Verificar permisos
            auto curso = CursoService::obtenerCursoPorId(cursoId);
            if (!curso || curso->getProfesorId() != userId)
            {
                throw std::runtime_error("No tienes permiso para esta acción");
            }
    
            bool success = CursoService::expulsarEstudiante(cursoId, estudianteId);
    
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Estudiante expulsado" : "Error al expulsar estudiante";

            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
        }

        // Actualizar nombre de curso
        else if (req->path() == "/profesor/cursos/nombre" && req->method() == Put) {
            auto cursoId = json->get("curso_id", 0).asInt();
            auto nuevoNombre = json->get("nombre", "").asString();

            auto curso = CursoService::obtenerCursoPorId(cursoId);
            if (!curso || curso->getProfesorId() != userId) {
                throw std::runtime_error("No tienes permiso para modificar este curso");
            }

            curso->setNombre(nuevoNombre);
            CursoService::saveCursos();

            Json::Value respJson;
            respJson["status"] = "success";
            respJson["message"] = "Nombre actualizado";
            callback(HttpResponse::newHttpJsonResponse(respJson));
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
            
            auto resp = drogon::HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
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
            if (clase == nullptr)
            {
                throw std::runtime_error("Error al crear clase, el curso no existe");
            }
            
            //ajustar respuesta del controlador
            Json::Value respData;
            respData["status"] = "success";
            

            Json::Value dataObj;
            dataObj["id"] = clase->getId();
            dataObj["titulo"] = clase->getTitulo();
            respData["data"] = dataObj;
            
            auto resp = HttpResponse::newHttpJsonResponse(respData);
            resp->setStatusCode(drogon::HttpStatusCode::k200OK);
            callback(resp);
        }


        //Eliminar clases ----------------
        else if(req->path() == "/profesor/clases" && req->method() == Delete)
        {
            auto claseId = json->get("clase_id", 0).asInt();

            auto claseExiste = ClaseService::obtenerClasePorId(claseId);

            if(!claseExiste){throw std::runtime_error("Esa clase no existe");}

            auto curso = CursoService::obtenerCursoPorId(claseExiste->getCursoId());
            if(!curso || curso->getProfesorId() != userId)
            {
                throw std::runtime_error("El profesor de esta clase no es ustes");
            }

            bool success = ClaseService::eliminarClase(claseId);
    
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "Clase eliminada correctamente" : "Error al eliminar clase";

            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
            callback(resp);
        }

         // Actualizar URL de clase -------------------------
        else if (req->path() == "/profesor/clases/url" && req->method() == Put) {
            auto claseId = json->get("clase_id", 0).asInt();
            auto nuevaUrl = json->get("url", "").asString();
    
                // Verificar permisos
            auto clase = ClaseService::obtenerClasePorId(claseId);
            if (!clase) 
            {
                throw std::runtime_error("Clase no encontrada");
            }
    
            auto curso = CursoService::obtenerCursoPorId(clase->getCursoId());
            if (!curso || curso->getProfesorId() != userId) 
            {      
                throw std::runtime_error("No tienes permiso para modificar esta clase");
            }   
    
            bool success = ClaseService::actualizarUrlClase(claseId, nuevaUrl);
    
            Json::Value respJson;
            respJson["status"] = success ? "success" : "error";
            respJson["message"] = success ? "URL actualizada" : "Error al actualizar URL";
    
            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp->setStatusCode(success ? k200OK : k500InternalServerError);
          callback(resp);
        }

        // Actualizar clase (nombre y contenido)
        else if (req->path() == "/profesor/clases" && req->method() == Put) 
        {
            auto claseId = json->get("clase_id", 0).asInt();
            auto nuevoTitulo = json->get("titulo", "").asString();
            auto nuevoContenido = json->get("contenido", "").asString();

            auto clase = ClaseService::obtenerClasePorId(claseId);
            if (!clase) throw std::runtime_error("Clase no encontrada");

            auto curso = CursoService::obtenerCursoPorId(clase->getCursoId());
            if (!curso || curso->getProfesorId() != userId) {
                throw std::runtime_error("No tienes permiso para modificar esta clase");
            }

            if (!nuevoTitulo.empty()) clase->setTitulo(nuevoTitulo);
            if (!nuevoContenido.empty()) clase->setContenido(nuevoContenido);

            ClaseService::saveClases();

            Json::Value respJson;
            respJson["status"] = "success";
            callback(HttpResponse::newHttpJsonResponse(respJson));
        }

        //VER CLASES
        else if (req->path() == "/profesor/mis-clases" && req->method() == Get) {
            try {
                // 1. Validar token y obtener ID del profesor
                auto profesorId = JWT::validarToken(req).value();
        
                // 2. Obtener todos los cursos del profesor
                auto cursos = CursoService::listarCursosPorProfesor(profesorId);
        
                // 3. Recopilar todas las clases de esos cursos
                Json::Value jsonClases(Json::arrayValue);
                for (const auto& curso : cursos) {
                    auto clases = ClaseService::listarClasesPorCurso(curso->getId());
                    for (const auto& clase : clases) {
                        Json::Value claseJson;
                        claseJson["id"] = clase->getId();
                        claseJson["titulo"] = clase->getTitulo();
                        claseJson["curso_nombre"] = curso->getNombre(); // Nombre del curso padre
                        claseJson["url_youtube"] = clase->getUrlYouTube();
                        jsonClases.append(claseJson);
                    }
                }
        
                // 4. Enviar respuesta
                auto resp = HttpResponse::newHttpJsonResponse(jsonClases);
                callback(resp);
        
            } catch (const std::exception& e) {
                Json::Value errorResp;
                errorResp["status"] = "error";
                errorResp["message"] = e.what();
                auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                resp->setStatusCode(drogon::k500InternalServerError);
                callback(resp);
            }
        }

        //VER NOTAS Y CLASES DE UN CURSO ESPECIFICO
      /*
         else if (req->path() == "/profesor/cursos/clases-notas" && req->method() == Get) 
        {
        try {
            // Validar token y obtener ID del profesor
            auto profesorId = JWT::validarToken(req).value();
            int cursoId = std::stoi(req->getParameter("curso_id"));

            // Verificar que el profesor es dueño del curso
            auto curso = CursoService::obtenerCursoPorId(cursoId);
            if (!curso || curso->getProfesorId() != profesorId) {
                throw std::runtime_error("No tienes permiso para acceder a este curso");
            }

            // Obtener clases del curso
            auto clases = ClaseService::listarClasesPorCurso(cursoId);

            // Obtener estudiantes y sus notas
            auto estudiantes = CursoService::listarEstudiantesInscritos(cursoId);
            Json::Value jsonNotas(Json::arrayValue);

            for (const auto& [estId, nombre] : estudiantes) {
                Json::Value estudianteJson;
                estudianteJson["estudiante_id"] = estId;
                estudianteJson["nombre"] = nombre;

                // Obtener notas del estudiante en el curso
                auto notas = CursoService::obtenerNotas(cursoId, estId);
                if (notas) 
                {
                    auto [n1, n2, n3, n4] = notas.value();
                    estudianteJson["notas"] = Json::objectValue;
                    estudianteJson["notas"]["n1"] = n1;
                    estudianteJson["notas"]["n2"] = n2;
                    estudianteJson["notas"]["n3"] = n3;
                    estudianteJson["notas"]["n4"] = n4;
                }

                jsonNotas.append(estudianteJson);
            }

            Json::Value respuesta;
            respuesta["clases"] = Json::arrayValue;
            for (const auto& c : clases) {
                Json::Value claseJson;
                claseJson["id"] = c->getId();
                claseJson["titulo"] = c->getTitulo();
                respuesta["clases"].append(claseJson);
            }

            respuesta["notas_estudiantes"] = jsonNotas;
            auto resp = HttpResponse::newHttpJsonResponse(respuesta);
            callback(resp);

        } catch (const std::exception& e) 
        {
            Json::Value errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = e.what();
            auto resp = HttpResponse::newHttpJsonResponse(errorResp);
            resp->setStatusCode(drogon::k400BadRequest);
            callback(resp);
        }
        }
        */

    } catch (const std::exception& e) {
        Json::Value errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(errorResp);
        resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        callback(resp);
    }
}