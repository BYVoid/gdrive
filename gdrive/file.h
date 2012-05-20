#ifndef gdrive_file_h
#define gdrive_file_h

#include "gdrive_types.h"
#include "request.h"

namespace GDrive {

class File {
public:
    static File parse_from_xml(string & xml);
    
    File();
    
    enum Type {
        FILE,
        FOLDER,
    };
    
//protected:
    string id;
    string title;
    string ctime, mtime, atime;
    Type type;
    
    Request request;
};

}

#endif
