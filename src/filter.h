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

#include <boost/xpressive/xpressive.hpp>
#include <map>
#include <vector>

#include "attributeinfo.h"

class Package;

class Filter
{
public:
    static void setattrs(std::string s);
    static void clearattrs();

    static bool cmp(const Package *lhs, const Package *rhs, AttributeEnum attr);
    static bool matchesre(const Package *a,
                          const boost::xpressive::sregex needle);
    static bool matches(const Package *a, const std::string needle);
    static bool notmatchesre(const Package *a,
                             const boost::xpressive::sregex needle);
    static bool notmatches(const Package *a, const std::string needle);

    static void assigncol(Package *a, AttributeEnum attr);

private:

    static std::vector<AttributeEnum> attrlist;

    static std::map<std::string, int> groups;
};

#endif // FILTER_H
