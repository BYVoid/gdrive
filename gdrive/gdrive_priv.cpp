#include "gdrive_types.h"
#include "gdrive_priv.h"
#include "auth.h"

using namespace GDrive;

string GDrivePriv::authorize(const string & email, const string & password)
{
    return Auth::instance().authorize(email, password);
}

Folder GDrivePriv::get_folder(const string & path)
{
    return * Folder::get_by_id("folder:0B2ibI5Xgooz8YkhTWXZtTVpDQ28");
}
