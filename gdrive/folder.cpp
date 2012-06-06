#include "gdrive_types.h"
#include "folder.h"
#include "request.h"
#include "utils.h"
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


Folder * Folder::make_folder(const string path)
{
    StrArray path_secs = Utils::str_split(path, "/");
    
    if (path_secs.size() <= 1) {
        throw runtime_error("Invalid path: " + path);
    }
    
    string folder = path_secs.back();
    string parent_path = path.substr(0, path.length() - folder.length());
    
    Folder * parent = NULL;
    
    try {
        parent = get_by_path(parent_path);
    } catch (runtime_error &e) {
        return NULL;
    }
    
    return parent->make_subfolder(folder);
}

Folder * Folder::make_subfolder(const string name)
{
    Request request;
    Dict attrs = request.make_sub_folder(id, name);
    if (request.get_error().length() > 0) {
        throw runtime_error(request.get_error());
    }
    File * folder = factory(attrs);
    assert(folder->type == File::FOLDER);
    FileCache::instance().save(folder, folder->path());
    this->add_child(folder);
    return (Folder *) folder;
}

void Folder::add_child(File * child)
{
    for (list<File *>::iterator i = children.begin(); i != children.end(); i++) {
        if (*i == child) {
            return;
        }
    }
    children.push_back(child);
}
