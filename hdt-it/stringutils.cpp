#include <QRegExp>

#include <StopWatch.hpp>
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
                .arg(QString::number((double)size / multiplier, 'f', 1))
                .arg(sizes[i]);
    }
    return QString("0");
}

QString stringutils::toQString(const char *str)
{

    QString out = QString::fromUtf8(str);

    return out;
}

QString stringutils::escapeHTML(QString in)
{
    return in.replace("<", "&lt;").replace(">", "&gt;");
}

void stringutils::cut(QString &in, int size)
{
    if(in.length()>size) {
        in = in.left(size)+"...";
    }
}
