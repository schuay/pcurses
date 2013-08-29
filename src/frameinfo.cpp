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

#include "frameinfo.h"

#include <assert.h>

using std::string;

FrameInfo::FrameInfo(FrameEnum t, uint termw, uint termh) : type(t)
{
    recalcinfo(termw, termh);
}

void FrameInfo::recalcinfo(uint termw, uint termh)
{
    const int leftpanewidth = 30;
    const int paneheight = termh;
    const int listpaneheight = (3 * paneheight) / 5;

    switch (type) {
    case FE_LIST:
        w = leftpanewidth;
        h = listpaneheight;
        x = 0;
        y = 0;
        hasborder = true;
        title = "Packages";
        break;
    case FE_INFO:
        w = termw - leftpanewidth + 1;
        h = paneheight;
        x = leftpanewidth - 1;
        y = 0;
        hasborder = true;
        title = "Info (press h for help)";
        break;
    case FE_QUEUE:
        w = leftpanewidth;
        h = paneheight - listpaneheight + 1;
        x = 0;
        y = listpaneheight - 1;
        hasborder = true;
        title = "Queue";
        break;
    case FE_STATUS:
        w = termw;
        h = 1;
        x = 0;
        y = termh - 1;
        hasborder = false;
        break;
    case FE_INPUT:
        w = termw;
        h = 3;
        x = 0;
        y = termh - 3;
        hasborder = true;
        title = "Input";
        break;
    case FE_HELP:
        w = termw - 10;
        h = 21; /* number of help items */
        x = (termw - w) / 2;
        y = 1;
        hasborder = true;
        title = "Help";
        break;
    default:
        assert(0);
    }

    if (termw > 80 + leftpanewidth) {
        /* three panes next to each other, adjust some values */
        switch (type) {
        case FE_LIST:
            h = paneheight;
            break;
        case FE_INFO:
            w = termw - leftpanewidth * 2 + 1;
            x = leftpanewidth * 2 - 2;
            break;
        case FE_QUEUE:
            h = paneheight;
            x = leftpanewidth - 1;
            y = 0;
            break;
        default:
            break;
        }
    }
}
