#ifndef gdrive_gdocfile_h
#define gdrive_gdocfile_h

#include "file.h"

namespace GDrive {

class GDocFile : public File {
    friend class File;
    
public:
    virtual ~GDocFile();
    virtual string name();
    
protected:
    GDocFile(Type type);
};

}

#endif
