#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <QString>

class stringutils
{
public:
    static QString sizeHuman(unsigned long long size);
    static QString cleanN3String(const char *str);
    static QString asRich(QString in);
    static void cut(QString &in, int size);
};



#endif // STRINGUTILS_HPP
