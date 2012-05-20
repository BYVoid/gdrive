#ifndef gdrive_request_h
#define gdrive_request_h

#include "gdrive_types.h"
#include "xml.h"

namespace GDrive {

class Request {
public:
    Request();
    ~Request();
    const string get_error();
    string do_request(const string url, Dict * fields, Dict * headers);
    Dict get_folder(const string & id);
    list<Dict> get_folder_contents(const string id);
    list<Dict> get_files_by_title(const string title);
    
//private:
    XmlNode get_resource(string url);
    
    string error;
};

}

#endif
