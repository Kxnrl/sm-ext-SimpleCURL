#include "extension.h"
#include "simplecurl.h"
#include <curl/curl.h>

SimpleCURL g_SimpleCURL;
SMEXT_LINK(&g_SimpleCURL);

extern const sp_nativeinfo_t g_spnatives[];

class CurlTypeHandler : public IHandleTypeDispatch
{
public:
    void OnHandleDestroy(HandleType_t type, void* object)
    {
        delete (SimpleCURL*)object;
    }
};

HandleType_t g_CurlHandleType;
CurlTypeHandler g_HandleType;

bool SimpleCURL::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    g_CurlHandleType = handlesys->CreateType("cURL", &g_HandleType, 0, nullptr, nullptr, myself->GetIdentity(), nullptr);

    curl_global_init(CURL_GLOBAL_ALL);

    // inited
    sharesys->RegisterLibrary(myself, "SimpleCURL");
    sharesys->AddNatives(myself, g_spnatives);

    return true;
}

void SimpleCURL::SDK_OnUnload()
{
    curl_global_cleanup();
}

static cell_t Curl_Curl(IPluginContext* pContext, const cell_t* params)
{
    char* url;
    pContext->LocalToString(params[1], &url);

    auto* curl = SimpleCurl::CreateHandle(url);
    if (!curl)
    {
        // null
        return pContext->ThrowNativeError("Failed to create Simple cURL handle.");
    }

    return handlesys->CreateHandle(g_CurlHandleType, (void*)curl, pContext->GetIdentity(), myself->GetIdentity(), nullptr);
}

static cell_t Curl_SetHeader(IPluginContext* pContext, const cell_t* params)
{
    HandleError err;
    HandleSecurity sec;
    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    SimpleCurl* curl;
    if ((err = handlesys->ReadHandle(static_cast<Handle_t>(params[1]), g_CurlHandleType, &sec, (void **)&curl)) != HandleError_None)
    {
        // can not access
        return pContext->ThrowNativeError("Invalid cURL handle %x (error %d)", params[1], err);
    }

    char* header;
    pContext->LocalToString(params[2], &header);

    char* value;
    pContext->LocalToString(params[3], &value);

    curl->SetHeader(header, value);

    return true;
}

static cell_t Curl_SetBody(IPluginContext* pContext, const cell_t* params)
{
    HandleError err;
    HandleSecurity sec;
    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    SimpleCurl* curl;
    if ((err = handlesys->ReadHandle(static_cast<Handle_t>(params[1]), g_CurlHandleType, &sec, (void**)&curl)) != HandleError_None)
    {
        // can not access
        return pContext->ThrowNativeError("Invalid cURL handle %x (error %d)", params[1], err);
    }

    char* body;
    pContext->LocalToString(params[2], &body);
    curl->SetBody(body);

    return true;
}

static cell_t Curl_Perform(IPluginContext* pContext, const cell_t* params)
{
    HandleError err;
    HandleSecurity sec;
    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    SimpleCurl* curl;
    if ((err = handlesys->ReadHandle(static_cast<Handle_t>(params[1]), g_CurlHandleType, &sec, (void**)&curl)) != HandleError_None)
    {
        // can not access
        return pContext->ThrowNativeError("Invalid cURL handle %x (error %d)", params[1], err);
    }

    return curl->Perform(params[2]);
}

static cell_t Curl_GetError(IPluginContext* pContext, const cell_t* params)
{
    HandleError err;
    HandleSecurity sec;
    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    SimpleCurl* curl;
    if ((err = handlesys->ReadHandle(static_cast<Handle_t>(params[1]), g_CurlHandleType, &sec, (void**)&curl)) != HandleError_None)
    {
        // can not access
        return pContext->ThrowNativeError("Invalid cURL handle %x (error %d)", params[1], err);
    }

    return pContext->StringToLocalUTF8(params[2], params[3], curl->GetError().c_str(), nullptr);
}

static cell_t Curl_GetCode(IPluginContext* pContext, const cell_t* params)
{
    HandleError err;
    HandleSecurity sec;
    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    SimpleCurl* curl;
    if ((err = handlesys->ReadHandle(static_cast<Handle_t>(params[1]), g_CurlHandleType, &sec, (void**)&curl)) != HandleError_None)
    {
        // can not access
        return pContext->ThrowNativeError("Invalid cURL handle %x (error %d)", params[1], err);
    }

    return curl->GetResponseCode();
}

static cell_t Curl_GetResponse(IPluginContext* pContext, const cell_t* params)
{
    HandleError err;
    HandleSecurity sec;
    sec.pOwner = pContext->GetIdentity();
    sec.pIdentity = myself->GetIdentity();

    SimpleCurl* curl;
    if ((err = handlesys->ReadHandle(static_cast<Handle_t>(params[1]), g_CurlHandleType, &sec, (void**)&curl)) != HandleError_None)
    {
        // can not access
        return pContext->ThrowNativeError("Invalid cURL handle %x (error %d)", params[1], err);
    }

    return pContext->StringToLocalUTF8(params[2], params[3], curl->GetResponseBody().c_str(), nullptr);
}

const sp_nativeinfo_t g_spnatives[] =
{
    {"CURL.CURL",               Curl_Curl},
    {"CURL.SetHeader",          Curl_SetHeader},
    {"CURL.SetBody",            Curl_SetBody},
    {"CURL.Perform",            Curl_Perform},
    {"CURL.GetError",           Curl_GetError},
    {"CURL.ResponseCode.get",   Curl_GetCode},
    {"CURL.GetResponseBody",    Curl_GetResponse},
    {nullptr, nullptr}
};
