#include "stringutils.hpp"

QString stringutils::sizeHuman(unsigned long long size)
{
    static const char     *sizes[]   = { "EB", "PB", "TB", "GB", "MB", "KB", "B" };
    static const unsigned long long exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;

    char result[20];
    unsigned long long multiplier = exbibytes;

    if(size==0) {
        strcpy(result, "0");
    } else {
        for (unsigned int i = 0; i < 7; i++, multiplier /= 1024)
        {
            if (size < multiplier)
                continue;
            if (size % multiplier == 0)
                sprintf(result, "%ju %s", (uintmax_t) (size / multiplier), sizes[i]);
            else
                sprintf(result, "%.2f %s", (float) size / multiplier, sizes[i]);
            break;
        }
    }
    return QString(result);
}
