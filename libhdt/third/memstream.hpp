/*
 * File: HDTManager.hpp
 * Last modified: $Date$
 * Revision: $Revision: 1 $
 * Last modified by: $Author$
 *
 * Copyright (C) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef LIBHDT_THIRD_MEMSTREAM_H
#define LIBHDT_THIRD_MEMSTREAM_H

#include <istream>
#include <ostream>
#include <streambuf>
// for EOF:
#include <cstdio>
// for memmove():
#include <cstring>


// low-level read and write functions
#ifdef _MSC_VER
# include <io.h>
#else
# include <unistd.h>
#endif

class memstream : public std::istream {
public:
    memstream(const char* buffer, size_t size)
        : std::istream(&_buffer), _buffer(buffer, size) {
        rdbuf(&_buffer);
    }

private:
    class Buffer : public std::streambuf {
    public:
        Buffer(const char* buffer, size_t size) {
            setg(const_cast<char*>(buffer), const_cast<char*>(buffer), const_cast<char*>(buffer + size));
        }

        std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override {
            if (way == std::ios_base::cur) {
                gbump(static_cast<int>(off));
            } else if (way == std::ios_base::end) {
                setg(eback(), egptr() + off, egptr());
            } else if (way == std::ios_base::beg) {
                setg(eback(), eback() + off, egptr());
            }
            return gptr() - eback();
        }

        std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override {
            return seekoff(sp, std::ios_base::beg, which);
        }
    };

    Buffer _buffer;
};

#endif  // LIBHDT_THIRD_MEMSTREAM_H
