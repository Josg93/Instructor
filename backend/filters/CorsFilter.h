#pragma once
#include <drogon/HttpFilter.h>
using namespace drogon;

class CorsFilter : public HttpFilter<CorsFilter> {
public:
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};
