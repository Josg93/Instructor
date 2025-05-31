#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class EstudianteController : public drogon::HttpSimpleController<EstudianteController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
        PATH_ADD ("/estudiante/perfil" , Put);
        PATH_ADD("/estudiante/cursos", Post);
        PATH_ADD("/estudiante/cursos", Get);
        PATH_ADD("/estudiante/cursos/salir", Delete);
        PATH_ADD("/estudiante/notas", Get);
        PATH_ADD("/estudiante/clases", Get);
        PATH_ADD("/estudiante/clases/ver" , Get);
    PATH_LIST_END
};
