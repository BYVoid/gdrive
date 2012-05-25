#ifndef gdrive_file_h
#define gdrive_file_h

#include "gdrive_types.h"
#include "request.h"

namespace GDrive {

class Folder;

class File {
    friend class Folder;

public:
    enum Type {
        FILE,
        FOLDER,
    };
    
    static File * factory(Dict & attrs);
    static list<File *> get_by_title(const string title);
    static File * get_by_path(const string path);
    
    File();
    virtual ~File();
    Folder * get_parent();
    Type get_type();
    virtual string to_json();
    
    //protected:
    
protected:
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
