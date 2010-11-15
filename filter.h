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

#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <algorithm>

#include "package.h"

using std::vector;
using std::string;

class Filter
{
public:
    static void setattrs(string s);
    static void clearattrs();

    static bool cmp(const Package *lhs, const Package *rhs);
    static bool matchesre(const Package *a, const sregex needle);
    static bool matches(const Package *a, const string needle);

private:

    static AttributeEnum chartoattr(const char c);

    static vector<AttributeEnum>
            attrlist;
};

#endif // FILTER_H
