#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class ProfesorController : public drogon::HttpSimpleController<ProfesorController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    PATH_ADD ("/profesor/perfil", Put);
    PATH_ADD("/profesor/cursos", Post);
    PATH_ADD("/profesor/cursos", Get);
    PATH_ADD("/profesor/cursos/abandonar", Post);
    PATH_ADD("/profesor/cursos" , Delete);
    PATH_ADD("/profesor/cursos/expulsar", Post);
    PATH_ADD("/profesor/cursos/nombre" , Put);
    //PATH_ADD("/profesor/cursos/{curso_id}/clases-notas", Get);
    PATH_ADD("/profesro/mis_clases", Get);
    PATH_ADD("/profesor/notas", Put);
    PATH_ADD("/profesor/clases", Post);
    PATH_ADD("/profesor/clases", Delete);
    PATH_ADD("/profesor/clases/url", Put);
    PATH_ADD("/profesor/clases", Put);
    PATH_LIST_END
};