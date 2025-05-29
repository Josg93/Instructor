#include "UsuarioController.h"
#include "UsuarioService.hpp"
#include "../utils/JWT.h"

void UsuarioController::asyncHandleHttpRequest(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback
) {
    try {
        
        auto usuarios = UsuarioService::listarUsuarios();
        Json::Value array(Json::arrayValue);
        
        for (const auto& u : usuarios) {
            Json::Value json;
            json["id"] = u->getId();
            json["nombre"] = u->getNombre();
            json["tipo"] = u->getTipo();
            array.append(json);
        }
        
        callback(HttpResponse::newHttpJsonResponse(array));
    } catch (const std::exception& e) {
        Json::Value errorResp;
        errorResp["status"] = "error";
        errorResp["message"] = e.what();
        callback(HttpResponse::newHttpJsonResponse(errorResp));
    }
}