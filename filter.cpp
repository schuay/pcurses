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

void Filter::clearattrs() {
    Filter::attrlist.clear();

    /* defaults */
    attrlist.push_back(A_NAME);
    attrlist.push_back(A_DESC);
}

void Filter::setattrs(string s) {
    AttributeEnum attr;

    Filter::attrlist.clear();

    for (unsigned int i = 0; i < s.length(); i++) {
        attr = chartoattr(s[i]);

        if (attr == A_NONE) continue;
        if (find(Filter::attrlist.begin(), Filter::attrlist.end(), attr) != Filter::attrlist.end()) continue;

        Filter::attrlist.push_back(attr);
    }
}

AttributeEnum Filter::chartoattr(const char c) {
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

bool Filter::matchesre(const Package *a, const sregex needle) {
    bool found = false;
    smatch what;

    for (unsigned int i = 0; i < Filter::attrlist.size(); i++)
        found = found || regex_search(a->getattr(Filter::attrlist[i]), what, needle);

    return !found;
}
bool Filter::matches(const Package *a, const string needle) {
    bool found = false;
    string str;

    string lneedle = needle;
    boost::to_lower(lneedle);

    for (unsigned int i = 0; i < Filter::attrlist.size(); i++) {
        str = a->getattr(Filter::attrlist[i]);
        boost::to_lower(str);
        found = found || str.find(lneedle) != std::string::npos;
    }

    return !found;
}
bool Filter::cmp(const Package *lhs, const Package *rhs) {
    return lhs->getname() < rhs->getname();
}
