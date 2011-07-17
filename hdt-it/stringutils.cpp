
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

        int precision = (size % multiplier == 0) ? 0 : 2;

        return QString("%1 %2")
                .arg(QString::number(size / multiplier, 'f', precision))
                .arg(sizes[i]);
    }
}
