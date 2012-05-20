#ifndef gdrive_
#define gdrive_

#include "gdrive_types.h"
#include "folder.h"

/* The classes below are exported */
#pragma GCC visibility push(default)

namespace GDrive {

class GDrive
{
public:
    GDrive();
    ~GDrive();
    string authorize(const string email, const string password);
    Folder get_folder(const string path);
    
private:
    void * priv;
};
    
}

#pragma GCC visibility pop
#endif
