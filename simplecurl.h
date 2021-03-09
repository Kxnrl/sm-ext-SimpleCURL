#ifndef _INCLUDE_SIMPLECURL_H_
#define _INCLUDE_SIMPLECURL_H_

#include <string>
#include <unordered_map>
#include <curl/curl.h>

class SimpleCurl
{
public:
    virtual ~SimpleCurl();
    static SimpleCurl* CreateHandle(const char* url);
    void SetHeader(const char* key, const char* value);
    void SetBody(const char* value);
    bool Perform(bool isPost);
    std::string GetError() { return curl_easy_strerror(this->code); }
    std::string GetResponseBody() { return responseBody; }
    int32_t GetResponseCode() { return responseCode; }

private:
    CURL* handle;
    std::unordered_map<std::string, std::string> headers;
    std::string requestBody;
    std::string responseBody;
    CURLcode code;
    int32_t responseCode;

    SimpleCurl(CURL* curl) : handle(curl) { }
};

#endif