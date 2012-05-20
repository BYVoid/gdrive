#ifndef gdrive_folder_h
#define gdrive_folder_h

#include "gdrive_types.h"
#include "file.h"

namespace GDrive {
    
class Folder : public File {
public:
    static Folder * get_by_id(const string & id);
    
    virtual ~Folder();
    void get_children();
    
//protected:
    Folder();
    
    bool children_retrieved;
    list<File *> children;
};

}

#endif
