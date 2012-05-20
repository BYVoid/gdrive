#include "utils.h"
#include <cstdio>

using namespace GDrive;

string Utils::read_from_file(const string filename)
{
    FILE * fp = fopen(filename.c_str(), "rb");
    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    string buf;
    buf.resize(len);
    fread((char *)buf.c_str(), len, 1, fp);
    fclose(fp);
    return buf;
}


StrArray Utils::str_split(string buffer, const string separator)
{
    StrArray array;
    size_t separator_len = separator.length();
    char * pos;
    const char * p_buffer = buffer.c_str();
    for (;;) {
        pos = strstr(p_buffer, separator.c_str());
        if (pos == NULL)
            break;
        *pos = '\0';
        array.push_back(p_buffer);
        p_buffer = pos + separator_len;
    };
    array.push_back(p_buffer);
    return array;
}
