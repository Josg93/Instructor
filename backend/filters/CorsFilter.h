#pragma once
#include <drogon/HttpFilter.h>

class CorsFilter : public drogon::HttpFilter<CorsFilter> 
{
    public:
        virtual void doFilter(const drogon::HttpRequestPtr& req, 
                             drogon::FilterCallback&& fcb,
                             drogon::FilterChainCallback&& fccb) override
        {
            if(req->method() == drogon::Options)
            {
                auto resp = drogon::HttpResponse::newHttpResponse();
                setCorsHeaders(resp);
                resp->setStatusCode(drogon::k200OK);
                fcb(resp);
                return;
            }
        }
    private:
     static void setCorsHeaders(const drogon::HttpResponsePtr& resp)
     {
        resp->addHeader("Access-Control-Allow-Origin", "http://localhost:3000");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, ContentType, Accept , Authorization");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
            resp->addHeader("Access-Control-Allow-Expose-Headers", "Authorization");
            resp->addHeader("Vary", "origin");
          //  resp->addHeader("Access-Control-Allow-Max-Age", "86400");
     }
};