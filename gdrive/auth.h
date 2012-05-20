#ifndef gdrive_auth_h
#define gdrive_auth_h

#include "gdrive_types.h"

namespace GDrive {

class Auth {
public:
    static Auth & instance();
    string authorize(const string & email, const string & password);
    string get_auth_string();
    
private:
    Auth();
    void parse_auth(const string & buffer);
    
    string auth_string;
    Dict auth_dict;
};

}

#endif
