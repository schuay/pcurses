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

#include "state.h"

#include <assert.h>

State::
State()
{
    mode = MODE_STANDARD;
    sortedby = A_NAME;
    coloredby = A_INSTALLSTATE;
    op = OP_NONE;
}

std::string optostr(FilterOperationEnum o)
{
    switch (o) {
    case OP_FILTER:
        return "/";
    case OP_SORT:
        return ".";
    case OP_SEARCH:
        return "?";
    case OP_COLORCODE:
        return ";";
    case OP_EXEC:
        return "!";
    case OP_MACRO:
        return "@";
    case OP_NONE:
        return "";
    default:
        assert(0);
    }

    return "";
}

FilterOperationEnum strtoopt(std::string str)
{
    for (int i = 0; i < OP_NONE; i++) {
        if (optostr((FilterOperationEnum)i) == str) {
            return (FilterOperationEnum)i;
        }
    }
    return OP_NONE;
}
