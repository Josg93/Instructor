#include "AuthController.h"
#include "UsuarioService.hpp"
#include "JWT.h"
#include <jwt-cpp/jwt.h>

void AuthController::asyncHandleHttpRequest(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback
) {
    Json::Value errorResp;
    errorResp["status"] = "error";

    try 
    {
        // Verificar que el cuerpo de la solicitud es JSON válido
        if (!req->getJsonObject()) 
        {
            errorResp["message"] = "Cuerpo de la solicitud no es JSON válido";
            auto resp = HttpResponse::newHttpJsonResponse(errorResp);
            resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
            return callback(resp);
        }

        auto json = req->getJsonObject();

        // Registro --------------------------------------------------------------------
        if (req->path() == "/auth/registro")
         {
            try 
            {
                // Validar campos obligatorios
                if (!json->isMember("nombre") || !json->isMember("email") || !json->isMember("password") || !json->isMember("tipo")) 
                {
                    errorResp["message"] = "Faltan campos obligatorios";
                    auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                    resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
                    return callback(resp);
                }

                auto nombre = json->get("nombre", "").asString();
                auto email = json->get("email", "").asString();
                auto password = json->get("password", "").asString();
                auto tipo = json->get("tipo", "").asString();
                
                bool success = UsuarioService::registrarUsuario(nombre, email, password, tipo);

                if (!success) 
                {
                    throw std::runtime_error("Error al registrar");
                }
                
                Json::Value respJson;
                respJson["status"] = "success";
                respJson["message"] = "Usuario registrado";
                
                auto resp = HttpResponse::newHttpJsonResponse(respJson);
                callback(resp);
            } catch(const std::exception& e) 
            {
                errorResp["message"] = e.what();
                auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
                callback(resp);
            }
        }
        // Login ---------------------------------------------------------------------------
        else if (req->path() == "/auth/login")
         {
            try {
                // Validar campos obligatorios
                if ((!json->isMember("nombre") && !json->isMember("email")) || !json->isMember("password")) 
                {
                    errorResp["message"] = "Se requiere nombre/email y contraseña";
                    auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                    resp->setStatusCode(drogon::HttpStatusCode::k400BadRequest);
                    return callback(resp);
                }

                auto nombre = json->get("nombre", "").asString();
                auto email = json->get("email", "").asString();
                auto password = json->get("password", "").asString();
                
                auto usuario = UsuarioService::autenticar(nombre, email, password);
                
                if (usuario) 
                {
                    auto token = jwt::create()
                        .set_issuer("auth0")
                        .set_type("JWS")
                        .set_payload_claim("id", jwt::claim(std::to_string(usuario.value()->getId())))
                        .set_payload_claim("tipo", jwt::claim(usuario.value()->getTipo()))
                        .sign(jwt::algorithm::hs256{"ROPAINTERIOR"});
                    
                    Json::Value respJson;
                    respJson["status"] = "success";
                    respJson["token"] = token;

                    auto resp = HttpResponse::newHttpJsonResponse(respJson);
                    resp->setStatusCode(drogon::HttpStatusCode::k200OK);
                    callback(resp);
                }
                else 
                {
                    errorResp["message"] = "Credenciales inválidas";
                    auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
                    callback(resp);
                }
            }
            catch(const std::exception& e) 
            {
                errorResp["message"] = e.what();
                auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
                callback(resp);
            }
        }

        // Eliminar cuenta (común para ambos roles)
        else if (req->path() == "/auth/cuenta" && req->method() == Delete) 
        {
            try {
                // Validar token JWT
                auto userId = JWT::validarToken(req);
                if (!userId) {
                    Json::Value errorResp;
                    errorResp["status"] = "error";
                    errorResp["message"] = "Token inválido o expirado";

                    auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                    resp->setStatusCode(drogon::HttpStatusCode::k401Unauthorized);
                    return callback(resp);
                }

                // Eliminar el usuario
                bool success = UsuarioService::eliminarUsuario(userId.value());

                Json::Value respJson;
                if (success) {
                    respJson["status"] = "success";
                    respJson["message"] = "Cuenta eliminada exitosamente";

                    auto resp = HttpResponse::newHttpJsonResponse(respJson);
                    resp->setStatusCode(drogon::HttpStatusCode::k200OK);
                    callback(resp);
                } else {
                    respJson["status"] = "error";
                    respJson["message"] = "No se pudo eliminar la cuenta";

                    auto resp = HttpResponse::newHttpJsonResponse(respJson);
                    resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
                    callback(resp);
                }
            } catch (const std::exception& e) {
                Json::Value errorResp;
                errorResp["status"] = "error";
                errorResp["message"] = e.what();

                auto resp = HttpResponse::newHttpJsonResponse(errorResp);
                resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
                callback(resp);
            }
        }
        // Ruta no encontrada
        else 
        {
            errorResp["message"] = "Ruta no encontrada";
            auto resp = HttpResponse::newHttpJsonResponse(errorResp);
            resp->setStatusCode(drogon::HttpStatusCode::k404NotFound);
            callback(resp);
        }
    } 
    catch(const std::exception& e) 
    {
        errorResp["message"] = "Error interno del servidor";
        auto resp = HttpResponse::newHttpJsonResponse(errorResp);
        resp->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        callback(resp);
    }
}