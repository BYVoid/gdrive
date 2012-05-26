#ifndef gdrive_utils_h
#define gdrive_utils_h

#include "gdrive_types.h"

#pragma GCC visibility push(hidden)

namespace GDrive {
    
class Utils
{
public:
    static string read_from_file(const string filename);
    static StrArray str_split(const string buffer, const string separator);
};

}

#pragma GCC visibility pop

#endif
