#include "gdrive_types.h"
#include "folder.h"
#include "request.h"
#include "filecache.h"

using namespace GDrive;

Folder::Folder()
{
    type = FOLDER;
    parent = NULL;
    children_retrieved = false;
}

Folder::~Folder()
{

}

string Folder::to_json()
{
    ostringstream buffer;
    buffer << '{';
    buffer << "\"id\": " << '"' << id << '"' << ',';
    buffer << "\"title\": " << '"' << title << '"' << ',';
    buffer << "\"parent_id\": " << '"' << parent_id << '"' << ',';
    buffer << "\"children\": " << '[';
    if (children_retrieved) {
        for (list<File *>::iterator i = children.begin(); i != children.end(); i++) {
            File * child = *i;
            buffer << child->to_json();
            if (child != children.back()) {
                buffer << ',';
            }
        }
    }
    buffer << ']';
    buffer << '}';
    return buffer.str();
}

Folder * Folder::get_by_id(const string id)
{
    File * file = File::get_by_id(id);
    if (file == NULL) {
        return NULL;
    }
    
    if (file->get_type() != File::FOLDER) {
        throw runtime_error("Not a folder.");
    }
    
    Folder * folder = (Folder *) file;
    folder->get_children();
    return folder;
}

list<File *> Folder::get_children()
{
    if (children_retrieved) {
        return children;
    }
    
    list<Dict> children_attrs = request.get_folder_contents(id);
    if (request.get_error().length() > 0) {
        throw runtime_error(request.get_error());
    }
    
    for (list<Dict>::iterator i = children_attrs.begin(); i != children_attrs.end(); i++) {
        File * child = factory(*i);
        child->parent = this;
        child->parent_id = id;
        children.push_back(child);
        FileCache::instance().save(child);
    }
    
    children_retrieved = true;
    return children;
}

Folder * Folder::get_by_path(const string path)
{
    File * file = File::get_by_path(path);
    if (file->type != File::FOLDER) {
        throw runtime_error("Not a folder");
    }
    return (Folder *) file;
}
