/*
 * File: HDTVersion.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 * This file determines the current version of the software.
 * The total version number is: v<HDT_version>.<INDEX_VERSION>.<RELEASE_VERSION>
 */

 #ifndef HDT_HDTVERSION_HPP_
 #define HDT_HDTVERSION_HPP_

// Version of the actual HDT file that is generated or read.
// Software must be backwards compatible with all HDT files with the same number.
#define HDT_VERSION "1"

// Version of the accompagning .index file that is generated or read
// Software must be backwards compatible with all index files with the same index and HDT version number.
#define INDEX_VERSION "1"

// Subreleases that are backwards compatible with both HDT and index file
#define RELEASE_VERSION "2"

#include <string>

namespace hdt {
namespace HDTVersion {
    inline std::string get_version_string(std::string delimiter) {
      return std::string("v") + HDT_VERSION + delimiter + INDEX_VERSION + delimiter + RELEASE_VERSION;
    };

    inline std::string get_index_suffix(std::string delimiter) {
      return std::string(".index.v") + HDT_VERSION + delimiter+INDEX_VERSION;
    };
}
}
#endif /* HDT_HDTVERSION_HPP_ */
