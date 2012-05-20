#include "file.h"
#include "folder.h"

using namespace GDrive;

File::File()
{
    type = FILE;
    parent = NULL;
}

File::~File()
{
    
}


File * File::factory(Dict & attrs)
{
    File * file = NULL;
    
    if (attrs["type"] == "folder") {
        file = new Folder();
    } else {
        file = new File();
    }
    
    new File();
    file->id = attrs["id"];
    file->title = attrs["title"];
    file->ctime = attrs["ctime"];
    file->mtime = attrs["mtime"];
    file->atime = attrs["atime"];
    file->parent_id = attrs["parent_id"];
    
    return file;
}

list<File *> File::get_by_title(const string title)
{
    list<File *> files;
    Request request;
    list<Dict> files_attrs = request.get_files_by_title(title);
    
    if (request.get_error().length() > 0) {
        throw runtime_error(request.get_error());
    }
    
    for (list<Dict>::iterator i = files_attrs.begin(); i != files_attrs.end(); i++) {
        File * file = factory(*i);
        files.push_back(file);
    }
    
    return files;
}
