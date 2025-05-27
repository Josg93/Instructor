#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class AuthController : public drogon::HttpSimpleController<AuthController>
{
  public:
    void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
        PATH_ADD("/auth/registro", Post);
        PATH_ADD("/auth/login", Post);
    PATH_LIST_END
};
