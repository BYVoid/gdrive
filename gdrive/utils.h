#ifndef gdrive_utils_h
#define gdrive_utils_h

#include "gdrive_types.h"

namespace GDrive {
    
class Utils
{
public:
    static string read_from_file(const string filename);
    static StrArray str_split(string buffer, const string separator);
};

}

#endif
