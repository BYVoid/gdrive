#include "gdrive_types.h"
#include "folder.h"
#include "request.h"

using namespace GDrive;

Folder::Folder()
{
    type = FOLDER;
    children_retrieved = false;
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
            return NULL;
        }
        folder = new Folder();
        folder->id = attributes["id"];
        folder->title = attributes["title"];
        folder->ctime = attributes["ctime"];
        folder->mtime = attributes["mtime"];
        folder->atime = attributes["atime"];
    }
    folder->get_children();
    return folder;
}

void Folder::get_children()
{
    list<Dict> children_attrs = request.get_folder_contents(id);
    
    if (request.get_error().length() > 0) {
        throw runtime_error(request.get_error());
        return;
    }
    
    for (list<Dict>::iterator i = children_attrs.begin(); i != children_attrs.end(); i++) {
        Dict & attr = *i;
        File * child;
        
        if (attr["type"] == "folder") {
            child = new Folder();
        } else {
            child = new File();
        }
        
        child->id = attr["id"];
        child->title = attr["title"];
        child->ctime = attr["ctime"];
        child->mtime = attr["mtime"];
        child->atime = attr["atime"];
        
        children.push_back(child);
    }
    
    children_retrieved = true;
}
