#include "gdrive_types.h"
#include "folder.h"
#include "request.h"

using namespace GDrive;

Folder::Folder()
{
    type = FOLDER;
    parent = NULL;
    children_retrieved = false;
}

Folder::~Folder()
{
    for (list<File *>::iterator i = children.begin(); i != children.end(); i++) {
        File * file = *i;
        delete file;
    }
}

Folder * Folder::get_by_id(const string & id)
{
    Folder * folder = NULL;
    if (id == "folder%3Aroot") {
        folder = new Folder();
        folder->id = id;
        folder->title = '/';
    } else {
        Request request;
        Dict attributes = request.get_folder(id);
        if (request.get_error().length() > 0) {
            throw runtime_error(request.get_error());
        }
        folder = new Folder();
        folder->id = attributes["id"];
        folder->title = attributes["title"];
        folder->ctime = attributes["ctime"];
        folder->mtime = attributes["mtime"];
        folder->atime = attributes["atime"];
        folder->parent_id = attributes["parent_id"];
    }
    folder->get_children();
    return folder;
}

void Folder::get_children()
{
    list<Dict> children_attrs = request.get_folder_contents(id);
    
    if (request.get_error().length() > 0) {
        throw runtime_error(request.get_error());
    }
    
    for (list<Dict>::iterator i = children_attrs.begin(); i != children_attrs.end(); i++) {
        File * child = factory(*i);
        child->parent = this;
        child->parent_id = id;
        children.push_back(child);
    }
    
    children_retrieved = true;
}
