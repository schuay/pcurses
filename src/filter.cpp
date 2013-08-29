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

#include "filter.h"

/* allocating static member
   http://stackoverflow.com/questions/272900/c-undefined-reference-to-static-class-member
 */
vector<AttributeEnum> Filter::attrlist;
map<string, int> Filter::groups;

void Filter::clearattrs()
{
    Filter::attrlist.clear();

    /* defaults */
    attrlist.push_back(A_NAME);
    attrlist.push_back(A_DESC);

    Filter::groups.clear();
}

void Filter::setattrs(string s)
{
    AttributeEnum attr;

    Filter::attrlist.clear();

    for (uint i = 0; i < s.length(); i++) {
        attr = AttributeInfo::chartoattr(s[i]);

        if (attr == A_NONE) {
            continue;
        }
        if (find(Filter::attrlist.begin(), Filter::attrlist.end(), attr) != Filter::attrlist.end()) {
            continue;
        }

        Filter::attrlist.push_back(attr);
    }
}

void Filter::assigncol(Package *a, AttributeEnum attr)
{
    string s = a->getattr(attr);
    int colindex;

    map<string, int>::iterator it = groups.find(s);

    if (it != groups.end()) {
        colindex = it->second;
    } else {
        colindex = groups.size();
        groups[s] = colindex;
    }

    a->setcolindex(colindex);
}

bool Filter::matches(const Package *a, const string needle)
{
    return !notmatches(a, needle);
}

bool Filter::matchesre(const Package *a, const sregex needle)
{
    return !notmatchesre(a, needle);
}

bool Filter::notmatchesre(const Package *a, const sregex needle)
{
    bool found = false;
    smatch what;

    for (uint i = 0; i < Filter::attrlist.size(); i++) {
        found = found || regex_search(a->getattr(Filter::attrlist[i]), what, needle);
    }

    return !found;
}

bool Filter::notmatches(const Package *a, const string needle)
{
    bool found = false;
    string str;

    string lneedle = needle;
    boost::to_lower(lneedle);

    for (uint i = 0; i < Filter::attrlist.size(); i++) {
        str = a->getattr(Filter::attrlist[i]);
        boost::to_lower(str);
        found = found || str.find(lneedle) != std::string::npos;
    }

    return !found;
}

bool Filter::cmp(const Package *lhs, const Package *rhs, AttributeEnum attr)
{
    if (attr == A_SIZE || attr == A_ISIZE || attr == A_BUILDDATE) {
        return lhs->getoffattr(attr) < rhs->getoffattr(attr);
    }

    return lhs->getattr(attr) < rhs->getattr(attr);
}
