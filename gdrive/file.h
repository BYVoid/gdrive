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
    
    static File * NOT_FOUND;
    
    static list<File *> get_by_title(const string title);
    static File * get_by_id(const string id);
    static File * get_by_path(const string path);
    
    string get_id() const;
    Folder * get_parent();
    Type get_type() const;
    virtual string to_json() const;
    virtual string name() const;
    virtual string contents();
    virtual size_t size();
    virtual string path();
    virtual void rename(const string new_path);
    
protected:
    static File * factory(Dict & attrs);
    
    File();
    virtual ~File();
    
    Type type;
    string id;
    string title;
    string ctime, mtime, atime;
    Folder * parent;
    string parent_id;
    string url;
    bool m_contents_retrieved;
    string m_contents;
    bool m_size_retrieved;
    size_t m_size;
    
    Request request;
    
};

}

#endif
