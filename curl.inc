#if defined __cURL_include__
    #endinput
#endif
#define __cURL_include__

#define cURL CURL

methodmap cURL < Handle
{
    public native cURL(const char[] url);

    public native bool SetHeader(const char[] header, const char[] value);
    public native bool SetBody(const char[] body);
    public native bool Perform(bool post = false);
    public native void GetError(char[] buffer, int maxLen);
    public native void GetResponseBody(char[] buffer, int maxLen);
    
    property int ResponseCode
    {
        public native get();
    }
}

public Extension __ext__SimpleCURL = 
{
    name = "SimpleCURL",
    file = "SimpleCURL.ext",
#if defined AUTOLOAD_EXTENSIONS
    autoload = 1,
#else
    autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
    required = 1,
#else
    required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_Kxnrl_Message_SetNTVOptional()
{
    MarkNativeAsOptional("CURL.CURL");
    MarkNativeAsOptional("CURL.SetHeader");
    MarkNativeAsOptional("CURL.SetBody");
    MarkNativeAsOptional("CURL.Perform");
    MarkNativeAsOptional("CURL.GetError");
    MarkNativeAsOptional("CURL.ReadShort");
    MarkNativeAsOptional("CURL.GetResponseBody");
    MarkNativeAsOptional("CURL.ResponseCode.get");
}
#endif