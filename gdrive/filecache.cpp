#include "filecache.h"

using namespace GDrive;

FileCache::FileCache()
{
    
}

FileCache & FileCache::instance()
{
    static FileCache * instance = new FileCache();
    return *instance;
}

File * FileCache::get_by_id(const string id)
{
    if (id_cache.count(id) == 1) {
        return id_cache[id];
    }
    return File::NOT_FOUND;
}

File * FileCache::get_by_path(const string path)
{
    if (path_cache.count(path) == 1) {
        return path_cache[path];
    }
    return File::NOT_FOUND;
}

void FileCache::save(File * file)
{
    string id = file->get_id();
    File * old_cache = get_by_id(id);
    if (old_cache != File::NOT_FOUND) {
        //delete old_cache;
        // TODO Fix this
    }
    id_cache[id] = file;
}

void FileCache::save(File * file, string path)
{
    if (file != NULL) {
        save(file);
    }
    if (path[path.length() - 1] == '/') {
        path = path.substr(0, path.length() - 1);
    }
    path_cache[path] = file;
}
