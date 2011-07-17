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

FrameInfo::FrameInfo(FrameEnum t, unsigned int termw, unsigned int termh) : type(t) {
    recalcinfo(termw, termh);
}

void FrameInfo::recalcinfo(unsigned int termw, unsigned int termh) {

    const int leftpanewidth = 30;
    const int paneheight = termh - 1;
    const int listpaneheight = (3 * paneheight) / 5;

    switch (type) {
    case FE_LIST:
        w = leftpanewidth;
        h = listpaneheight;
        x = 0;
        y = 0;
        hasborder = true;
        break;
    case FE_INFO:
        w = termw - leftpanewidth + 1;
        h = paneheight;
        x = leftpanewidth - 1;
        y = 0;
        hasborder = true;
        break;
    case FE_QUEUE:
        w = leftpanewidth;
        h = paneheight - listpaneheight;
        x = 0;
        y = listpaneheight;
        hasborder = true;
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
        y = termh - 2;
        hasborder = true;
        break;
    case FE_HELP:
        w = termw - 10;
        h = 19; /* number of help items */
        x = (termw - w) / 2;
        y = 1;
        hasborder = true;
        break;
    default:
        assert(0);
    }
}
