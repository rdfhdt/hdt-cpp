
#include "stringutils.hpp"

QString stringutils::sizeHuman(unsigned long long size)
{
    static const char     *sizes[]   = { "EB", "PB", "TB", "GB", "MB", "KB", "B" };
    static const quint64 exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;

    quint64 multiplier = exbibytes;

    if(size==0) {
        return QString("0");
    }

    for (unsigned int i = 0; i < 7; i++, multiplier /= 1024)
    {
        if (size < multiplier) {
            continue;
        }

        return QString("%1 %2")
                .arg(QString::number(size / multiplier, 'f', 2))
                .arg(sizes[i]);
    }
}
