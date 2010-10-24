/***************************************************************************

    Copyright 2010 Jakob Gruber

    This file is part of dungeons.

    dungeons is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    dungeons is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with dungeons.  If not, see <http://www.gnu.org/licenses/>.

 ***************************************************************************/

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

int CursesListBox::FocusedIndex() const {
    return windowpos + cursorpos;
}

void CursesListBox::Refresh() {
    Package *pkg;

    for (int i = 0; i <= UsableHeight(); i++) {
        if (windowpos + i > (int)list->size() - 1)
            break;

        pkg = list->at(windowpos + i);

        InstallReasonEnum installreason = pkg->reason();
        string installreasonstr;
        if (installreason == IRE_NOTINSTALLED) installreasonstr = '-';
        else if (installreason == IRE_EXPLICIT) installreasonstr = 'E';
        else installreasonstr = 'D';

        string updateavailable;
        if (pkg->needsupdate()) updateavailable = 'U';
        else updateavailable = '-';

        int attr = 0;
        if (installreason == IRE_EXPLICIT) attr = COLOR_PAIR(2);
        else if (installreason == IRE_ASDEPS) attr = COLOR_PAIR(3);
        if (i == cursorpos) attr = COLOR_PAIR(1);

        string pkgstring = "[" + installreasonstr + updateavailable + "] " +
                           pkg->name();

        MvPrintW(0, i, pkgstring.substr(0, UsableWidth() + 1), attr);
    }

    CursesFrame::Refresh();
}
