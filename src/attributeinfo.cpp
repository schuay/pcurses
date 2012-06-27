/* *************************************************************************
 *  Copyright 2010 Jakob Gruber                                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */

#include "attributeinfo.h"

AttributeEnum AttributeInfo::chartoattr(char c)
{
    c = tolower(c);

    switch (c) {
    case 'a': return A_SIGNATURE;
    case 'b': return A_BUILDDATE;
    case 'c': return A_DESC;
    case 'd': return A_UPDATESTATE;
    case 'e': return A_DEPENDS;
    case 'f': return A_CONFLICTS;
    case 'g': return A_GROUPS;
    case 'h': return A_ARCH;
    case 'i': return A_ISIZE;
    case 'k': return A_PACKAGER;
    case 'l': return A_LICENSES;
    case 'n': return A_NAME;
    case 'o': return A_OPTDEPENDS;
    case 'p': return A_PROVIDES;
    case 'r': return A_REPO;
    case 's': return A_REPLACES;
    case 't': return A_INSTALLSTATE;
    case 'u': return A_URL;
    case 'v': return A_VERSION;
    case 'z': return A_SIZE;
    default: return A_NONE;
    }
}

char AttributeInfo::attrtochar(AttributeEnum attr)
{
    for (char i = 'a'; i <= 'z'; i++)
        if (chartoattr(i) == attr) return i;

    throw PcursesException("Attribute has no assigned char.");
}

string AttributeInfo::attrname(AttributeEnum attr)
{
    switch(attr) {
    case A_ARCH: return "Arch";
    case A_BUILDDATE: return "Build date";
    case A_CONFLICTS: return "Conflicts";
    case A_DEPENDS: return "Depends";
    case A_DESC: return "Desc";
    case A_GROUPS: return "Groups";
    case A_INSTALLSTATE: return "Install state";
    case A_ISIZE: return "Install size";
    case A_LICENSES: return "Licenses";
    case A_NAME: return "Name";
    case A_OPTDEPENDS: return "Optdepends";
    case A_PACKAGER: return "Packager";
    case A_PROVIDES: return "Provides";
    case A_REPLACES: return "Replaces";
    case A_REPO: return "Repo";
    case A_SIGNATURE: return "Signature";
    case A_SIZE: return "Download size";
    case A_UPDATESTATE: return "Update state";
    case A_URL: return "Url";
    case A_VERSION: return "Version";
    case A_NONE: return "";
    default: throw PcursesException("Invalid attribute passed.");
    }
}
