#include "gdrive_types.h"
#include "gdrive.h"
#include "gdrive_priv.h"

using GDrive::Folder;

GDrive::GDrive::GDrive()
{
    priv = new GDrivePriv();
}

GDrive::GDrive::~GDrive()
{
    delete (GDrivePriv *) priv;
}

string GDrive::GDrive::authorize(const string email, const string password)
{
    GDrivePriv & obj = *(GDrivePriv *) priv;
    return obj.authorize(email, password);
}


Folder GDrive::GDrive::get_folder(const string path)
{
    GDrivePriv & obj = *(GDrivePriv *) priv;
    return obj.get_folder(path);
}
