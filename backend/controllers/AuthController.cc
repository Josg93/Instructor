#include "AuthController.h"
#include "UsuarioService.hpp"
#include <jwt-cpp/jwt.h>


void AuthController::asyncHandleHttpRequest(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback
) {


    auto json = req->getJsonObject();
    

    // Registro --------------------------------------------------------------------
    if (req->path() == "/auth/registro") {
        auto nombre = json->get("nombre", "").asString();
        auto email = json->get("email" , "").asString();
        auto password = json->get("password", "").asString();
        auto tipo = json->get("tipo", "").asString();
        
        bool success = UsuarioService::registrarUsuario(nombre,email,password, tipo);
        
        Json::Value respJson;
        respJson["status"] = success ? "success" : "error";
        respJson["message"] = success ? "Usuario registrado" : "Error al registrar";
        
        auto resp = HttpResponse::newHttpJsonResponse(respJson);
        if (! success)
            resp->setStatusCode(drogon::HttpStatusCode::k409Conflict);

        
        callback(resp);
    }

    //login ---------------------------------------------------------------------------
    else if (req->path() == "/auth/login") {
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
            resp -> setStatusCode(drogon::HttpStatusCode::k200OK);
            callback(resp);
        } else 
        {
            Json::Value respJson;
            respJson["status"] = "error";
            respJson["message"] = "Credenciales inválidas";
            auto resp = HttpResponse::newHttpJsonResponse(respJson);
            resp -> setStatusCode(drogon::HttpStatusCode::k409Conflict);
            callback(resp);
        }
    }
}