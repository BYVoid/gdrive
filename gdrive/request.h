#ifndef gdrive_request_h
#define gdrive_request_h

#include "gdrive_types.h"
#include "xml.h"

#pragma GCC visibility push(hidden)

namespace GDrive {

class Request {
    friend class Auth;
public:
    Request();
    ~Request();
    const string get_error();
    string get_contents(const string url);
    Dict get_folder(const string id);
    Dict get_file(const string id);    
    list<Dict> get_folder_contents(const string id);
    list<Dict> get_files_by_title(const string title);
    
private:
    string do_request(const string url, Dict * fields, Dict * headers);
    XmlNode get_resource(string url);
    
    string error;
};

}

#pragma GCC visibility pop

#endif
