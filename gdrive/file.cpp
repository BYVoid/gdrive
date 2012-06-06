#include "file.h"
#include "folder.h"
#include "gdocfile.h"
#include "utils.h"
#include "filecache.h"

using namespace GDrive;

File * File::NOT_FOUND = (File *) -1;

File::File()
{
    type = FILE;
    parent = NULL;
    m_contents_retrieved = false;
}

File::~File()
{
    
}

string File::get_id() const
{
    return id;
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

File::Type File::get_type() const
{
    return type;
}

string File::to_json() const
{
    ostringstream buffer;
    buffer << '{';
    buffer << "\"id\": " << '"' << id << '"' << ',';
    buffer << "\"title\": " << '"' << title << '"' << ',';
    buffer << "\"parent_id\": " << '"' << parent_id << '"';
    buffer << '}';
    return buffer.str();
}

string File::name() const
{
    return title;
}

string File::contents()
{
    if (!m_contents_retrieved) {
        m_contents = request.get_contents(url);
        m_contents_retrieved = true;
    }
    return m_contents;
}

size_t File::size()
{
    if (!m_size_retrieved) {
        m_size = request.get_length(url);
        m_size_retrieved = true;
    }
    return m_size;
}

string File::path()
{
    StrArray path_secs;
    File * cur = this;
    while (cur->parent_id != "") {
        path_secs.push_back(cur->name());
        cur = cur->get_parent();
    }
    ostringstream buffer;
    buffer << '/';
    for (StrArray::reverse_iterator i = path_secs.rbegin(); i != path_secs.rend(); i++) {
        buffer << *i << '/';
    }
    return buffer.str();
}

void File::rename(const string new_path)
{
    // TODO rename
}

File * File::factory(Dict & attrs)
{
    File * file = NULL;
    
    if (attrs.count("error")) {
        throw new runtime_error(attrs["error"]);
    }
    
    string id = attrs["id"];
    
    if (attrs["type"] == "folder") {
        file = new Folder();
    } else if (id.compare(0, 8, "document") == 0) {
        file = new GDocFile(DOCUMENT);
    } else if (id.compare(0, 7, "drawing") == 0) {
        file = new GDocFile(DRAWING);
    } else if (id.compare(0, 11, "spreadsheet") == 0) {
        file = new GDocFile(SPREADSHEET);
    } else if (id.compare(0, 12, "presentation") == 0) {
        file = new GDocFile(PRESENTATION);
    } else {
        file = new File();
    }
    
    file->id = id;
    file->title = attrs["title"];
    file->ctime = attrs["ctime"];
    file->mtime = attrs["mtime"];
    file->atime = attrs["atime"];
    file->parent_id = attrs["parent_id"];
    file->url = attrs["url"];
    
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
        FileCache::instance().save(file);
    }
    
    return files;
}

File * File::get_by_id(const string id)
{
    File * file = FileCache::instance().get_by_id(id);
    if (file != File::NOT_FOUND) {
        return file;
    }
    
    if (id == "folder:root") {
        file = new Folder();
        file->id = id;
        file->title = "";
        file->parent_id = "";
        file->parent = (Folder *) -1;
        FileCache::instance().save(file, "/");
    } else {
        Request request;
        Dict attrs = request.get_folder(id);
        if (request.get_error().length() > 0) {
            throw runtime_error(request.get_error());
        }
        file = factory(attrs);
        FileCache::instance().save(file);
    }
    return file;
}

File * File::get_by_path(const string path)
{
    File * file = FileCache::instance().get_by_path(path);
    if (file != File::NOT_FOUND) {
        return file;
    }
    
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
    filename = Utils::filter_extension(filename);
    
    path_secs.pop_back();
    list<File *> files = get_by_title(filename);
    list<File *> files_cands = files;
    
    // filter files iterately by parent
    int depth = 1;
    do {
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
    } while (files_cands.size() > 1 && path_secs.size() >= 1);
    
    file = NULL;
    // target file found
    if (files_cands.size() == 1) {
        file = files_cands.front();
    }
    
    // cache all other files
    for (list<File *>::iterator i = files.begin(); i != files.end(); i++) {
        File * file_cand = *i;
        if (file_cand != file) { // except for the target file
            FileCache::instance().save(file_cand);
        }
    }
    
    FileCache::instance().save(file, path);
    
    return file;
}
