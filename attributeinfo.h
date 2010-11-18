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

#ifndef ATTRIBUTEINFO_H
#define ATTRIBUTEINFO_H

#include <string>

#include "alpmexception.h"

using std::string;

enum AttributeEnum {
    A_NAME,
    A_VERSION,
    A_URL,
    A_REPO,
    A_PACKAGER,
    A_BUILDDATE,
    A_INSTALLSTATE,
    A_DESC,
    A_ARCH,
    A_LICENSES,
    A_GROUPS,
    A_DEPENDS,
    A_OPTDEPENDS,
    A_CONFLICTS,
    A_PROVIDES,
    A_REPLACES,
    A_SIZE,
    A_ISIZE,
    A_NONE,
};

class AttributeInfo
{
public:
    static AttributeEnum chartoattr(char c);
    static char attrtochar(AttributeEnum attr);
    static string attrname(AttributeEnum attr);
};

#endif // ATTRIBUTEINFO_H
