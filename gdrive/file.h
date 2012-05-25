#ifndef gdrive_file_h
#define gdrive_file_h

#include "gdrive_types.h"
#include "request.h"

namespace GDrive {

class Folder;
class GDocFile;

class File {
    friend class Folder;

public:
    enum Type {
        FILE,
        FOLDER,
        DOCUMENT,
        DRAWING,
        SPREADSHEET,
        PRESENTATION,
    };
    
    static File * factory(Dict & attrs);
    static list<File *> get_by_title(const string title);
    static File * get_by_path(const string path);
    
    File();
    virtual ~File();
    Folder * get_parent();
    Type get_type();
    virtual string to_json();
    virtual string name();
    
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
