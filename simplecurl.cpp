#include <simplecurl.h>
#include <curl/curl.h>

size_t CurlResponseCallback(char* contents, size_t size, size_t nmemb, std::string* userp)
{
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

SimpleCurl::~SimpleCurl()
{
    curl_easy_cleanup(handle);
}

SimpleCurl* SimpleCurl::CreateHandle(const char* url)
{
    auto* curl = curl_easy_init();
    if (!curl)
        return nullptr;

    curl_easy_setopt(curl, CURLOPT_URL, url);

    return new SimpleCurl(curl);
}

void SimpleCurl::SetHeader(const char* key, const char* value)
{
    this->headers[key] = value;
}

void SimpleCurl::SetBody(const char* value)
{
    requestBody = value;
}

bool SimpleCurl::Perform(bool isPost)
{
    struct curl_slist* slist = nullptr;
    if (!this->headers.empty())
    {
        std::string header;

        for (auto it = this->headers.begin(); it != this->headers.end(); ++it)
        {
            if (!it->first.empty()) {
                header = it->first + ":";
            }
            header += it->second;
            slist = curl_slist_append(slist, header.c_str());

            if (strcmp(it->first.c_str(), "Accept-Encoding") == 0)
            {
                // encoding...
                curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, it->second.c_str());
            }
            else if (strcmp(it->first.c_str(), "User-Agent") == 0)
            {
                // UA
                curl_easy_setopt(handle, CURLOPT_USERAGENT, it->second.c_str());
            }
        }

        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, slist);
    }

    if (!requestBody.empty())
    {
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, requestBody.c_str());
    }

    curl_easy_setopt(handle, isPost ? CURLOPT_POST : CURLOPT_HTTPGET, 1L);

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResponseCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &this->responseBody);

    this->code = curl_easy_perform(handle);

    if (slist != nullptr)
    {
        curl_slist_free_all(slist);
    }

    if (this->code != CURLE_OK)
    {
        // -1 is not valid http code
        return false;
    }

    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &this->responseCode);

    return true;
}
