#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <QString>
#include <string>

class stringutils
{
public:
    static QString sizeHuman(unsigned long long size);
    static QString toQString(const char *str);
    static QString escapeHTML(QString in);
    static void cut(QString &in, int size);
};



#endif // STRINGUTILS_HPP
