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

CursesListBox::CursesListBox(int w, int h, int x, int y, bool hasborder)
    : CursesFrame(w, h, x, y, hasborder),
      windowpos(0),
      cursorpos(0)
{
}

void CursesListBox::SetList(vector<Package*> *l) {
    list = l;
    UpdateFocus();
}

void CursesListBox::Move(int step) {
    MoveAbs(FocusedIndex() + step);
}

void CursesListBox::MoveAbs(int pos) {
    if (list->size() == 0)
        return;

    if (pos < 0) pos = 0;
    else if (pos >= (int)list->size()) pos = (int)list->size() - 1;

    /* target visible, do not scroll window */
    if (pos >= windowpos && pos <= windowpos + UsableHeight()) {
        cursorpos = pos - windowpos;
    /* scrolling required (up) */
    } else if (pos < windowpos) {
        windowpos = pos;
        cursorpos = 0;
    /* scrolling required (down) */
    } else {
        windowpos = pos - UsableHeight();
        if (windowpos < 0) windowpos = 0;
        cursorpos = pos - windowpos;
    }
}

void CursesListBox::MoveEnd() {
    MoveAbs(list->size() - 1);
}

bool CursesListBox::IsInBounds(int pos) const {
    if (pos < 0)
        return false;
    if ((unsigned int)(pos) >= list->size())
        return false;
    return true;
}

void CursesListBox::UpdateFocus() {
    int lsize = (int)list->size();

    if (FocusedIndex() >= lsize) {
        if (lsize - UsableHeight() < 0)
            windowpos = 0;
        else
            windowpos = lsize - UsableHeight();

        cursorpos = lsize - windowpos - 1;
    }
}

chtype CursesListBox::GetCol(int index) const {
    const int colnum = 7; /* nr of defined col pairs */
    int col = index % colnum;

    switch (col) {
    case 0: return C_DEF;
    case 1: return C_DEF_HL1;
    case 2: return C_DEF_HL2;
    case 3: return C_DEF_HL3;
    case 4: return C_DEF_HL4;
    case 5: return C_DEF_HL5;
    case 6: return C_DEF_HL6;
    default: assert(0);
    }
}

int CursesListBox::FocusedIndex() const {
    return windowpos + cursorpos;
}

void CursesListBox::Refresh() {
    Package *pkg;

    for (int i = 0; i <= UsableHeight(); i++) {
        if (windowpos + i > (int)list->size() - 1)
            break;

        pkg = list->at(windowpos + i);

        int attr = GetCol(pkg->getcolindex());
        if (i == cursorpos) attr |= A_REVERSE;

        MvPrintW(0, i, pkg->getname().substr(0, UsableWidth() + 1), attr);
    }

    CursesFrame::Refresh();
}
