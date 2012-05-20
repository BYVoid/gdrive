#pragma GCC visibility push(hidden)

#include "request.h"
#include "folder.h"

namespace GDrive {

class GDrivePriv
{
public:
    string authorize(const string & email, const string & password);
    Folder get_folder(const string & path);
    
private:
    void parse_auth(const string & buffer);
    
    Request request;
    Dict auth_dict;
    string auth_string;
};

}

#pragma GCC visibility pop
