#pragma once

#include <drogon/HttpSimpleController.h>
#include <json/json.h>


using namespace drogon;

class UsuarioController : public drogon::HttpSimpleController<UsuarioController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
      PATH_ADD("/usuarios", Get);
    PATH_LIST_END
};
