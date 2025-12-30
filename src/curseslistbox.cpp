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

#include "curseslistbox.h"

#include <boost/format.hpp>

#include "package.h"

using std::vector;

CursesListBox::CursesListBox(FrameInfo *frameinfo)
    : CursesFrame(frameinfo),
      list(nullptr),
      windowpos(0),
      cursorpos(0)
{
}

void CursesListBox::setlist(vector<Package *> *l)
{
    list = l;
    updatefocus();
}

void CursesListBox::move(int step)
{
    moveabs(focusedindex() + step);
}

void CursesListBox::removeselected()
{
    size_t pos = focusedindex();

    if (!isinbounds(pos)) {
        return;
    }

    if (pos == list->size() - 1) {
        move(-1);
    }
    list->erase(list->begin() + pos);
}

void CursesListBox::moveabs(int pos)
{
    if (list->size() == 0) {
        return;
    }

    if (pos < 0) {
        pos = 0;
    } else if (pos >= (int)list->size()) {
        pos = (int)list->size() - 1;
    }

    /* target visible, do not scroll window */
    if (pos >= windowpos && pos <= windowpos + usableheight()) {
        cursorpos = pos - windowpos;
        /* scrolling required (up) */
    } else if (pos < windowpos) {
        windowpos = pos;
        cursorpos = 0;
        /* scrolling required (down) */
    } else {
        windowpos = pos - usableheight();
        if (windowpos < 0) {
            windowpos = 0;
        }
        cursorpos = pos - windowpos;
    }
}

void CursesListBox::movetoend()
{
    moveabs(list->size() - 1);
}

bool CursesListBox::isinbounds(int pos) const
{
    if (pos < 0) {
        return false;
    }
    if ((uint)(pos) >= list->size()) {
        return false;
    }
    return true;
}

void CursesListBox::updatefocus()
{
    int lsize = (int)list->size();

    if (focusedindex() >= lsize) {
        if (lsize - usableheight() < 0) {
            windowpos = 0;
        } else {
            windowpos = lsize - usableheight();
        }

        cursorpos = lsize - windowpos - 1;
    }
}

chtype CursesListBox::getcol(int index) const
{
    const int colnum = 7; /* nr of defined col pairs */
    int col = index % colnum;

    switch (col) {
    case 0:
        return C_DEF;
    case 1:
        return C_DEF_HL1;
    case 2:
        return C_DEF_HL2;
    case 3:
        return C_DEF_HL3;
    case 4:
        return C_DEF_HL4;
    case 5:
        return C_DEF_HL5;
    case 6:
        return C_DEF_HL6;
    default:
        assert(0);
    }

    return C_DEF;
}

int CursesListBox::focusedindex() const
{
    return windowpos + cursorpos;
}

Package *CursesListBox::focusedpackage() const
{
    if (list->size() == 0) {
        return nullptr;
    }
    if (!isinbounds(focusedindex())) {
        return nullptr;
    }

    return list->at(focusedindex());
}

void CursesListBox::refresh()
{
    Package *pkg;

    setheader(boost::str(boost::format("(%d)") % list->size()));

    for (int i = 0; i <= usableheight(); i++) {
        if (windowpos + i > (int)list->size() - 1) {
            break;
        }

        pkg = list->at(windowpos + i);

        int attr = getcol(pkg->getcolindex());
        if (i == cursorpos) {
            attr |= A_REVERSE;
        }

        mvprintw(0, i, pkg->getname().substr(0, usablewidth() + 1), attr);
    }

    CursesFrame::refresh();
}
