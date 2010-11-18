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

AttributeEnum AttributeInfo::chartoattr(const char c) {
    switch (c) {
    case 'a': return A_PACKAGER;
    case 'b': return A_BUILDDATE;
    case 'c': return A_CONFLICTS;
    case 'd': return A_DESC;
    case 'e': return A_DEPENDS;
    case 'g': return A_GROUPS;
    case 'h': return A_ARCH;
    case 'i': return A_ISIZE;
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
char AttributeInfo::attrtochar(const AttributeEnum attr) {
    for (char i = 'a'; i <= 'z'; i++)
        if (chartoattr(i) == attr) return i;

    throw AlpmException("Attribute has no assigned char.");
}

string AttributeInfo::attrname(AttributeEnum attr) {
    switch(attr) {
    case A_NAME: return "Name";
    case A_VERSION: return "Version";
    case A_URL: return "Url";
    case A_REPO: return "Repo";
    case A_PACKAGER: return "pAckager";
    case A_BUILDDATE: return "Builddate";
    case A_INSTALLSTATE: return "install sTate";
    case A_DESC: return "Desc";
    case A_ARCH: return "arcH";
    case A_LICENSES: return "Licenses";
    case A_GROUPS: return "Groups";
    case A_DEPENDS: return "dEpends";
    case A_OPTDEPENDS: return "Optdepends";
    case A_CONFLICTS: return "Conflicts";
    case A_PROVIDES: return "Provides";
    case A_REPLACES: return "replaceS";
    case A_SIZE: return "download siZe";
    case A_ISIZE: return "Install size";
    case A_NONE: return "";
    default: throw AlpmException("Invalid attribute passed.");
    }
}
