#include "settings.h"
#include "gdrive.h"
#include <fuse.h>
#include <fuse/fuse_opt.h>
#include <errno.h>

using namespace GDrive;

struct gdrivefs_config {
    char * email;
    char * password;
};

struct gdrivefs_config conf;

void * gdrivefs_init(struct fuse_conn_info *conn)
{
    Auth::instance().authorize(conf.email, conf.password);
    return NULL;
}

int gdrivefs_getattr(const char * path, struct stat * stbuf)
{
    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
    }
    else {
        File * file = File::get_by_path(path);
        if (file == NULL) {
            return -ENOENT;
        }
        
        stbuf->st_mode = S_IFREG | 0444;
        if (file->get_type() == File::FOLDER) {
            stbuf->st_mode = S_IFDIR | 0777;
        }
        
        stbuf->st_nlink = 1;
        stbuf->st_size = file->size();
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
    
    size_t len = file->size();
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

int gdrivefs_rename(const char * path_from, const char * path_to)
{
    File * file = File::get_by_path(path_from);
    if (file == NULL) {
        return -ENOENT;
    }
    
    try {
        file->rename(path_to);
    } catch (runtime_error & err) {
        return -ENOENT;
    }
    
    return 0;
}

int gdrivefs_mkdir(const char * path, mode_t mode)
{
    Folder * folder = Folder::make_folder(path);
    if (folder == NULL) {
        return -ENOENT;
    }
    return 0;
}

int gdrivefs_unlink(const char * path)
{
    File * file = File::get_by_path(path);
    if (file == NULL) {
        return -ENOENT;
    }
    file->remove();
    return 0;
}

static struct fuse_operations gdrivefs_oper = {
    .init = gdrivefs_init,
    .getattr = gdrivefs_getattr,
    .readdir = gdrivefs_readdir,
    .open = gdrivefs_open,
    .read = gdrivefs_read,
    .rename = gdrivefs_rename,
    .mkdir = gdrivefs_mkdir,
    .unlink = gdrivefs_unlink,
    .rmdir = gdrivefs_unlink,
};

enum {
    KEY_HELP,
    KEY_VERSION,
};

#define GDRIVEFS_OPT(t, p, v) { t, offsetof(struct gdrivefs_config, p), v }

static struct fuse_opt gdrivefs_opts[] = {
    GDRIVEFS_OPT("--password=%s", password, 0),
    GDRIVEFS_OPT("--email=%s", email, 0),
    FUSE_OPT_KEY("-V", KEY_VERSION),
    FUSE_OPT_KEY("--version", KEY_VERSION),
    FUSE_OPT_KEY("-h", KEY_HELP),
    FUSE_OPT_KEY("--help", KEY_HELP),
    FUSE_OPT_END
};

static int gdrivefs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
    switch (key) {
        case KEY_HELP:
            fprintf(stderr,
                    "usage: %s mountpoint [options]\n"
                    "\n"
                    "GDriveFS options (required):\n"
                    "    --email=EMAIL           email address of your google account\n"
                    "    --password=PASSWORD     password of your google account\n"
                    "\n"
                    "general options:\n"
                    "    -o opt,[opt...]        mount options\n"
                    "    -h   --help            print help\n"
                    "    -V   --version         print version\n"
                    "\n",
                    outargs->argv[0]
            );
            fuse_opt_add_arg(outargs, "-ho");
            fuse_main(outargs->argc, outargs->argv, &gdrivefs_oper, NULL);
            exit(1);
            
        case KEY_VERSION:
            fprintf(stderr, "GDriveFS version %s\n", VERSION);
            fuse_opt_add_arg(outargs, "--version");
            fuse_main(outargs->argc, outargs->argv, &gdrivefs_oper, NULL);
            exit(0);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    memset(&conf, 0, sizeof(conf));

    fuse_opt_parse(&args, &conf, gdrivefs_opts, gdrivefs_opt_proc);
    if (conf.email == NULL || conf.password == NULL) {
        fprintf(stderr, "Must specify email address and password. Use --help to get help.\n");
        return 1;
    }
    
    return fuse_main(args.argc, args.argv, &gdrivefs_oper, NULL);
}
