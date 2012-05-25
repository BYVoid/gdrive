#include "gdocfile.h"

using namespace GDrive;

GDocFile::GDocFile(Type type)
{
    this->type = type;
}

GDocFile::~GDocFile()
{
    
}

string GDocFile::name()
{
    string ext;
    switch (type) {
        case File::DOCUMENT:
            ext = ".gdoc";
            break;
        case File::PRESENTATION:
            ext = ".gslides";
            break;
        case File::DRAWING:
            ext = ".gdraw";
            break;
        case File::SPREADSHEET:
            ext = ".gsheet";
            break;
        default:
            assert(false);
    }
    return title + ext;
}
