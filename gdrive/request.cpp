#include <curl/curl.h>
#include "request.h"
#include "utils.h"
#include "auth.h"

using namespace GDrive;


static size_t write_memory_callback(char *data, size_t size, size_t nmemb, string *buffer)
{
	size_t result = 0;
	if (buffer != NULL) {
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

Request::Request()
{

}

Request::~Request()
{

}


const string Request::get_error()
{
    return error;
}

string Request::do_request(const string url, Dict * fields, Dict * headers)
{
    CURL *curl;
    CURLcode res;
    struct curl_httppost * formpost = NULL;
    struct curl_httppost * lastptr = NULL;
    struct curl_slist * headerlist = NULL;
    string buffer;
    error.clear();
    
    curl = curl_easy_init();
    if (!curl) {
        error = "cURL initialization failed";
        return "";
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    
    // Generate post fields
    if (fields != NULL) {
        for (Dict::const_iterator i = fields->begin(); i != fields->end(); i++) {
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_COPYNAME, i->first.c_str(),
                         CURLFORM_COPYCONTENTS, i->second.c_str(),
                         CURLFORM_END);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    }
    
    // Generate custom headers
    if (headers != NULL) {
        for (Dict::const_iterator i = headers->begin(); i != headers->end(); i++) {
            string header = i->first + ": ";
            header += i->second;
            headerlist = curl_slist_append(headerlist, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    }
    
    // Perform query
    res = curl_easy_perform(curl);
    
    if (res != 0) {
        error = curl_easy_strerror(res);
        buffer = "";
    }
    
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headerlist);
    
    return buffer;
}

string Request::get_contents(const string url)
{
    Dict headers;
    headers["GData-Version"] = "3.0";
    headers["Authorization"] = "GoogleLogin auth=" + Auth::instance().get_auth_string();
    if (error.length() > 0) {
        throw runtime_error(error);
    }
    return do_request(url, NULL, &headers);
}

XmlNode Request::get_resource(string url)
{
    return XmlNode::parse(get_contents(url));
}

Dict Request::get_folder(const string id)
{
    if (id.compare(0, 7, "folder:") != 0) {
        throw new runtime_error("Invalid folder id");
    }
    
    string url = "https://docs.google.com/feeds/default/private/full/folder%3A";
    url += id.c_str() + 7;
    XmlNode root = get_resource(url);
    
    Dict attrs;
    
    if (root.name() ==  "errors") {
        XmlNode reason_node = root.child("error").child("internalReason");
        if (reason_node.is_null()) {
            attrs["error"] = "Unknown error";
        } else {
            attrs["error"] = reason_node.contents();
        }
        return attrs;
    }
    
    attrs["title"] = root.child("title").contents();
    attrs["id"] = root.child("resourceId").contents();
    attrs["ctime"] = root.child("published").contents();
    attrs["mtime"] = root.child("edited").contents();
    attrs["atime"] = root.child("lastViewed").contents();
    
    return attrs;
}

Dict parse_entry(XmlNode & entry)
{
    Dict attrs;
    attrs["title"] = entry.child("title").contents();
    attrs["id"] = entry.child("resourceId").contents();
    attrs["ctime"] = entry.child("published").contents();
    attrs["mtime"] = entry.child("edited").contents();
    attrs["atime"] = entry.child("lastViewed").contents();
    attrs["url"] = entry.child("content").attr("src");
    
    string raw_type = entry.child("content").attr("type");
    if (raw_type == "application/atom+xml;type=feed") {
        attrs["type"] = "folder";
    } else {
        attrs["type"] = "file";
    }
    
    string parent_id = "root";
    list<XmlNode> link_nodes = entry.children("link");
    for (list<XmlNode>::iterator i = link_nodes.begin(); i != link_nodes.end(); i++) {
        XmlNode & node = *i;
        if (node.attr("rel") == "http://schemas.google.com/docs/2007#parent") {
            parent_id = node.attr("href");
            const char * pos = strstr(parent_id.c_str(), "folder%3A");
            assert(pos != NULL);
            parent_id = pos + 9;
        }
    }
    attrs["parent_id"] = "folder:" + parent_id;
    return attrs;
}

Dict Request::get_file(const string id)
{
    string url = "https://docs.google.com/feeds/default/private/full/" + id;
    XmlNode root = get_resource(url);
    
    Dict attrs;
    
    if (root.name() ==  "errors") {
        XmlNode reason_node = root.child("error").child("internalReason");
        if (reason_node.is_null()) {
            attrs["error"] = "Unknown error";
        } else {
            attrs["error"] = reason_node.contents();
        }
        return attrs;
    }
    
    attrs = parse_entry(root);
    
    return attrs;
}

list<Dict> parse_entries(XmlNode & node)
{
    list<Dict> files;
    list<XmlNode> entires_nodes = node.children("entry");
    for (list<XmlNode>::iterator i = entires_nodes.begin(); i != entires_nodes.end(); i++) {
        XmlNode & entry = *i;
        Dict attrs = parse_entry(entry);
        files.push_back(attrs);
    }
    return files;
}

list<Dict> Request::get_folder_contents(const string id)
{
    string url = "https://docs.google.com/feeds/default/private/full/" + id + "/contents";
    XmlNode root = get_resource(url);
    return parse_entries(root);
}

list<Dict> Request::get_files_by_title(const string title)
{
    //TODO escape title
    string url = "https://docs.google.com/feeds/default/private/full?title-exact=true&showfolders=true&title=" + title;
    XmlNode root = get_resource(url);
    return parse_entries(root);
}
