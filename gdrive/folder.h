#ifndef gdrive_folder_h
#define gdrive_folder_h

#include "gdrive_types.h"
#include "file.h"

namespace GDrive {
    
class Folder : public File {
    friend class File;
    
public:
    static Folder * get_by_id(const string id);
    static Folder * get_by_path(const string path);
    static Folder * make_folder(const string path);
    
    virtual ~Folder();
    list<File *> get_children();
    virtual string to_json();
    Folder * make_subfolder(const string name);
    
protected:
    Folder();
    
    bool children_retrieved;
    list<File *> children;
    void add_child(File * child);
};

}

#endif
