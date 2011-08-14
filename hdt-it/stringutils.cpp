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

QString stringutils::cleanN3String(const char *str)
{

    QString out = QString::fromUtf8(str);

    // If string is literal
    if(out.length()>0 && out.at(0)=='"') {
        // Replace unicode escape codes by the unicode characters themselves
        QRegExp rx("(\\\\u[0-9a-fA-F]{4})");
        int pos = 0;
        while ((pos = rx.indexIn(out, pos)) != -1) {
            out.replace(pos++, 6, QChar(rx.cap(1).right(4).toUShort(0, 16)));
        }
    }

#if 0
    // Remove <> from uri
    if(out.length()>1) {
        if(out.at(0)=='<') {
            out = out.remove(0,1);
        }
        if(out.at(out.length()-1)=='>') {
            out = out.remove(out.length()-1,1);
        }
    }
#endif
    return out;
}

QString stringutils::asRich(QString in)
{
    return in.replace("<", "&lt;").replace(">", "&gt;");
}

void stringutils::cut(QString &in, int size)
{
    if(in.length()>size) {
        in = in.left(size)+"...";
    }
}
