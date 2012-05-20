#ifndef gdrive_file_h
#define gdrive_file_h

#include "gdrive_types.h"
#include "request.h"

namespace GDrive {

class Folder;

class File {
public:
    static list<File *> get_by_title(const string title);
    static File * factory(Dict & attrs);
    
    File();
    virtual ~File();
    
    enum Type {
        FILE,
        FOLDER,
    };
    
    //protected:
    Type type;
    string id;
    string title;
    string ctime, mtime, atime;
    Folder * parent;
    string parent_id;
    
    Request request;
};

}

#endif
