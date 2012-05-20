#include <libxml/parser.h>
#include <libxml/tree.h>
#include "xml.h"

using namespace GDrive;

const string INVALID_XML = "Invalid XML Format";

XmlNode XmlNode::parse(string xml)
{
    xmlDocPtr doc = xmlReadMemory(xml.c_str(), xml.length(), "noname.xml", NULL, 0);
    if (doc == NULL)
        throw runtime_error(INVALID_XML);
    
    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (root == NULL) {
        xmlFreeDoc(doc);
        throw runtime_error(INVALID_XML);
    }
    
    XmlNode node;
    node.xml = xml;
    node.doc = doc;
    node.node = root;
    
    return node;
}

XmlNode::XmlNode():
    doc(NULL),
    node(NULL)
{
    
}

XmlNode::~XmlNode()
{
    free_doc();
}

void XmlNode::free_doc() {
    if (doc != NULL) {
        xmlFreeDoc((xmlDocPtr) doc);
    }
}

bool XmlNode::is_null() const
{
    return node == NULL;
}

string XmlNode::name() const
{
    return (const char *) ((xmlNodePtr) node)->name;
}

string XmlNode::contents() const
{
    if (node == NULL)
        return "";
    return (const char *) xmlNodeGetContent((xmlNodePtr) node);
}

string XmlNode::attr(string name) const
{
    if (node == NULL)
        return "";
    xmlChar * attr = xmlGetProp((xmlNodePtr) node, (const xmlChar *) name.c_str());
    if (attr == NULL)
        return "";
    return (const char *) attr;
}

XmlNode XmlNode::child(string name) const
{
    XmlNode child_node;
    if (node == NULL)
        return child_node;
    for (xmlNodePtr cur = ((xmlNodePtr) node)->children; cur != NULL; cur = cur->next) {
        if (name.compare((const char *) cur->name) == 0) {
            child_node.node = cur;
            return child_node;
        }
    }
    return child_node;
}

list<XmlNode> XmlNode::children() const
{
    list<XmlNode> children;
    if (node == NULL)
        return children;
    for (xmlNodePtr cur = ((xmlNodePtr) node)->children; cur != NULL; cur = cur->next) {
        XmlNode child_node;
        child_node.node = cur;
        children.push_back(child_node);
    }
    return children;
}

list<XmlNode> XmlNode::children(string name) const
{
    list<XmlNode> children;
    if (node == NULL)
        return children;
    for (xmlNodePtr cur = ((xmlNodePtr) node)->children; cur != NULL; cur = cur->next) {
        if (name.compare((const char *) cur->name) == 0) {
            XmlNode child_node;
            child_node.node = cur;
            children.push_back(child_node);
        }
    }
    return children;
}
