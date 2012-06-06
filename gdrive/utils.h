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
    static string url_escape(const string url);
    static string get_main_filename(const string filename);
    static string get_extension(const string filename);
    static string filter_extension(const string filename);
    static string to_string(const int num);
};

}

#pragma GCC visibility pop

#endif
