#include "file.h"
#include "folder.h"
#include "utils.h"

using namespace GDrive;

File::File()
{
    type = FILE;
    parent = NULL;
}

File::~File()
{
    
}

Folder * File::get_parent()
{
    if (this->parent != NULL) {
        return this->parent;
    }
    
    // TODO memory leak
    this->parent = Folder::get_by_id(this->parent_id);
    
    return this->parent;
}

File::Type File::get_type()
{
    return type;
}

string File::to_json()
{
    ostringstream buffer;
    buffer << '{';
    buffer << "\"id\": " << '"' << id << '"' << ',';
    buffer << "\"title\": " << '"' << title << '"' << ',';
    buffer << "\"parent_id\": " << '"' << parent_id << '"';
    buffer << '}';
    return buffer.str();
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

File * File::get_by_path(const string path)
{
    if (path == "/") {
        return Folder::get_by_id("folder:root");
    }
    
    StrArray path_secs = Utils::str_split(path, "/");
    
    if (path_secs.back() == "") { // path ended by '/'
        path_secs.pop_back();
    }
    
    if (path_secs.size() <= 1) {
        throw runtime_error("Invalid path: " + path);
    }
    
    string filename = path_secs.back();
    path_secs.pop_back();
    list<File *> files = get_by_title(filename);
    list<File *> files_cands = files;
    
    // filter files iterately by parent
    int depth = 1;
    while (files_cands.size() > 1 && path_secs.size() >= 1) {
        list<File *> files_cands_new;
        string parent_name = path_secs.back();
        for (list<File *>::iterator i = files_cands.begin(); i != files_cands.end(); i++) {
            File * file_cand = *i;
            // TODO memory leak optmize root
            Folder * parent = file_cand->get_parent();
            for (int i = 1; i < depth; i++) {
                parent = parent->get_parent();
            }
            if (parent->title == parent_name) {
                files_cands_new.push_back(file_cand);
            }
        }
        path_secs.pop_back();
        files_cands = files_cands_new;
        depth++;
    }
    
    File * file = NULL;
    // target file found
    if (files_cands.size() == 1) {
        file = files_cands.front();
    }
    
    // release memory allocated by File::get_by_title
    for (list<File *>::iterator i = files.begin(); i != files.end(); i++) {
        File * file_cand = *i;
        if (file_cand != file) { // except for the target file
            delete file_cand;
        }
    }
    
    return file;
}
