#include "fuse_settings.h"
#include "gdrive.h"
#include <fuse.h>
#include <errno.h>
#include <fstream>

using namespace GDrive;

int gdrivefs_getattr(const char * path, struct stat * stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 2;
    }
    else {
        File * file = File::get_by_path(path);
        if (file == NULL) {
            return -ENOENT;
        }
        
        stbuf->st_mode = S_IFREG | 0444;
        if (file->get_type() == File::FOLDER) {
            stbuf->st_mode = S_IFDIR | 0555;
        }
        
        stbuf->st_nlink = 1;
        stbuf->st_size = file->contents().length();
    }
    
    return 0;
}

int gdrivefs_readdir(const char * path, void * buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info * fi)
{
    (void) offset;
    (void) fi;
    
    Folder * folder = Folder::get_by_path(string(path));
    if (folder == NULL) {
        return -ENOENT;
    }
    
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    
    list<File *> files = folder->get_children();
    for (list<File *>::iterator i = files.begin(); i != files.end(); i++) {
        File * file = *i;
        filler(buf, file->name().c_str(), NULL, 0);
    }
    
    return 0;
}

int gdrivefs_open(const char * path, struct fuse_file_info * fi)
{
    File * file = File::get_by_path(path);
    if (file == NULL) {
        return -ENOENT;
    }
    
    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;
    
    return 0;
}

int gdrivefs_read(const char * path, char * buf, size_t size, off_t offset, struct fuse_file_info * fi)
{
    (void) fi;
    
    File * file = File::get_by_path(path);
    if (file == NULL) {
        return -ENOENT;
    }
    
    size_t len = file->contents().length();
    if (offset < len) {
        if (offset + size > len) {
            size = len - offset;
        }
        memcpy(buf, file->contents().c_str() + offset, size);
    } else {
        size = 0;
    }
    
    return size;
}

static struct fuse_operations hello_oper = {
    .getattr = gdrivefs_getattr,
    .readdir = gdrivefs_readdir,
    .open = gdrivefs_open,
    .read = gdrivefs_read,
};

int main(int argc, char * argv[])
{
    string email, password;
    std::ifstream user_file("user.txt");
    user_file >> email >> password;
    
    Auth::instance().authorize(email, password);
    return fuse_main(argc, argv, &hello_oper, NULL);
}
