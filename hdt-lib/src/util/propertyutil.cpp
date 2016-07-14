// Java-Style Properties in C++
//
// (c) Paul D. Senzee
// Senzee 5
// http://senzee.blogspot.com
#include <stdexcept>
#include "propertyutil.h"

#include <sstream>
#include <fstream>
//#include <exception>

void PropertyUtil::read(const char *filename, PropertyMapT &map)
{
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("unable to open properties file");
    read(file, map);
    file.close();
}

void PropertyUtil::read(std::istream &is, PropertyMapT &map)
{
    if (!is)
        throw std::runtime_error("unable to read from stream");

    char ch = 0;

    ch = is.get();

    while (!is.eof())
    {
        switch (ch)
        {
        case '#' :
        case '!' :
            do ch = is.get();
            while (!is.eof() && ch >= 0 && ch != '\n' && ch != '\r');
            continue;
        case '\n':
        case '\r':
        case ' ' :
        case '\t': ch = is.get(); continue;
        }

        // Read the key
        std::ostringstream key, val;

        while (!is.eof() && ch >= 0 && ch != '=' && ch != ':' &&
               ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r')
        {
            key << ch;
            ch = is.get();
        }

        while (!is.eof() && (ch == ' ' || ch == '\t'))
            ch = is.get();

        if (!is.eof() && (ch == '=' || ch == ':'))
            ch = is.get();

        while (!is.eof() && (ch == ' ' || ch == '\t'))
            ch = is.get();

        // Read the value
        while (!is.eof() && ch >= 0 && ch != '\n' && ch != '\r')
        {
            int next = 0;
            if (ch == '\\')
            {
                ch = is.get();
                switch (ch)
                {
                case '\r':
                    ch = is.get();
                    if (ch != '\n' && ch != ' ' && ch != '\t')
                        continue;
                    // fall through
                case '\n':
                    ch = is.get();
                    while (!is.eof() && (ch == ' ' || ch == '\t')) { is >> ch; }
                    continue;
                case 't': ch = '\t'; next = is.get(); break;
                case 'n': ch = '\n'; next = is.get(); break;
                case 'r': ch = '\r'; next = is.get(); break;
                case 'u':
                    {
                        ch = is.get();
                        while (!is.eof() && ch == 'u') { is >> ch; }
                        int d = 0;
                    loop:
                        for (int i = 0 ; !is.eof() && i < 4 ; i++)
                        {
                            next = is.get();
                            switch (ch)
                            {
                            case '0': case '1': case '2': case '3': case '4':
                            case '5': case '6': case '7': case '8': case '9':           d = (d << 4) +      ch - '0'; break;
                            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': d = (d << 4) + 10 + ch - 'a'; break;
                            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': d = (d << 4) + 10 + ch - 'A'; break;
                            default:                                                                                  goto loop;
                            }
                            ch = is.get();
                        }
                        ch = d;
                        break;
                    }
                default: next = is.get(); break;
                }
            }
            else
                next = is.get();

            val << ch;
            ch = next;
        }
        map[key.str()] = val.str();
    }
}

void PropertyUtil::write(const char *filename, PropertyMapT &map, const char *header)
{
    std::ofstream file(filename);
    write(file, map, header);
    file.close();
}

void PropertyUtil::write(std::ostream &os, PropertyMapT &map, const char *header)
{
    if (header != NULL)
        os << '#' << header << std::endl;

    os << '#';
    os << " <date> " << std::endl;

    for (iterator it = map.begin(), end = map.end(); it != end; ++it)
    {
        const std::string &key = (*it).first,
                          &val = (*it).second;
        os << key << '=';

        bool empty = false;

        for (size_t i = 0, sz = val.size(); i < sz; i++)
        {
            char ch = val[i];

            switch (ch)
            {
            case '\\': os << '\\' << '\\'; break;
            case '\t': os << '\\' << 't';  break;
            case '\n': os << '\\' << 'n';  break;
            case '\r': os << '\\' << 'r';  break;
            default:
                if (ch < ' ' || ch >= 127 || (empty && ch == ' '))
                    os << '\\' << 'u'
                       << m_hex((ch >> 12) & 0x0f) << m_hex((ch >> 8) & 0x0f)
                       << m_hex((ch >>  4) & 0x0f) << m_hex((ch >> 0) & 0x0f);
                else
                    os << ch;
            }

            empty = false;
        }
        os << std::endl;
    }
}

void PropertyUtil::print(std::ostream &os, PropertyMapT &map)
{
    iterator it = map.begin(), end = map.end();
    for (; it != end; ++it)
        os << (*it).first << "=" << (*it).second << std::endl;
}
