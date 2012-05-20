#ifndef gdrive_xml_h
#define gdrive_xml_h

#include "gdrive_types.h"

namespace GDrive {

class XmlNode {
public:
    static XmlNode parse(string xml);
    
    XmlNode();
    ~XmlNode();
    bool is_null() const;
    string name() const;
    string contents() const;
    string attr(string name) const;
    XmlNode child(string name) const;
    list<XmlNode> children() const;
    list<XmlNode> children(string name) const;
    
private:
    void free_doc();
    
    string xml;
    void * node;
    void * doc;
};

}

#endif
