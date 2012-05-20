#include "auth.h"
#include "request.h"
#include "utils.h"

using namespace GDrive;

const string LOGIN_URL = "https://www.google.com/accounts/ClientLogin";
const string SERVICE_NAME = "writely";
const string SOURCE_NAME = "byvoid-gdrivefs-0.0";
const string ACCOUNT_TYPE = "HOSTED_OR_GOOGLE";

Auth & Auth::instance()
{
    static Auth * instance = new Auth();
    return *instance;
}

Auth::Auth() :
    auth_string("")
{
    
}

void Auth::parse_auth(const string & buffer)
{
    StrArray attributes = Utils::str_split(buffer, "\n");
    size_t len = attributes.size();
    for (int i = 0; i < len; i++) {
        StrArray item = Utils::str_split(attributes[i], "=");
        if (item.size() != 2)
            break;
        auth_dict[item[0]] = item[1];
    }
}

string Auth::authorize(const string & email, const string & password)
{
    auth_string.clear();
    
    Dict posts;
    posts["accountType"] = ACCOUNT_TYPE;
    posts["Email"] = email;
    posts["Passwd"] = password;
    posts["service"] = SERVICE_NAME;
    posts["source"] = SOURCE_NAME;
    
    Request request;
    string buffer = request.do_request(LOGIN_URL, &posts, NULL);
    
    if (request.get_error().length() > 0)
        return request.get_error();
    
    parse_auth(buffer);
    if (auth_dict.count("Error") == 1)
        return auth_dict["Error"];
    
    if (auth_dict.count("Auth") == 0)
        return "Authorization protocol error";
    
    auth_string = auth_dict["Auth"];
    return "";
}

string Auth::get_auth_string()
{
    return auth_string;
}
