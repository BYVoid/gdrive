#ifndef gdrive_filecache_h
#define gdrive_filecache_h

#include "gdrive_types.h"
#include "file.h"

#pragma GCC visibility push(hidden)

namespace GDrive {
    
class FileCache {
public:
    static FileCache & instance();
    File * get_by_id(const string id);
    File * get_by_path(const string path);
    void save(File * file);
    void save(File * file, string path);
    void remove(File * file);
    
private:
    FileCache();
    map<string, File *> id_cache, path_cache;
    
};
    
}

#pragma GCC visibility pop


#endif
