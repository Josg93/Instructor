#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class ProfesorController : public drogon::HttpSimpleController<ProfesorController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    PATH_ADD("/profesor/cursos", Post);
    PATH_ADD("/profesor/cursos", Get);
    PATH_ADD("/profesor/notas", Put);
    PATH_ADD("/profesor/clases", Post);
    PATH_LIST_END
};