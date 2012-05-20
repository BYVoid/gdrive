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

XmlNode Request::get_resource(string & url)
{
    Dict headers;
    headers["GData-Version"] = "3.0";
    headers["Authorization"] = "GoogleLogin auth=" + Auth::instance().get_auth_string();
    string xml = do_request(url, NULL, &headers);
    if (error.length() > 0) {
        throw runtime_error(error);
    }
    return XmlNode::parse(xml);
}

Dict Request::get_folder(const string & id)
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

list<Dict> Request::get_folder_contents(const string & id)
{
    string url = "https://docs.google.com/feeds/default/private/full/" + id + "/contents";
    XmlNode root = get_resource(url);
    list<Dict> children;
    
    list<XmlNode> children_nodes = root.children("entry");
    for (list<XmlNode>::iterator i = children_nodes.begin(); i != children_nodes.end(); i++) {
        XmlNode & child = *i;
        
        Dict attrs;
        attrs["title"] = child.child("title").contents();
        attrs["id"] = child.child("resourceId").contents();
        attrs["ctime"] = child.child("published").contents();
        attrs["mtime"] = child.child("edited").contents();
        attrs["atime"] = child.child("lastViewed").contents();
        
        string raw_type = child.child("content").attr("type");
        if (raw_type == "application/atom+xml;type=feed") {
            attrs["type"] = "folder";
        } else {
            attrs["type"] = "file";
        }
        children.push_back(attrs);
    }
    return children;
}
