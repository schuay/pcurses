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

#include "cursesframe.h"

CursesFrame::CursesFrame(int w, int h, int x, int y, bool hasborder)
    : w_main(NULL), w_border(NULL)
{
    if (hasborder) {
        w_border = newwin(h, w, x, y);
        w_main = newwin(h - 2, w - 2, x + 1, y + 1);
    } else {
        w_main = newwin(h, w, x, y);
    }
}

CursesFrame::~CursesFrame()
{
    if (w_border != NULL)
        delwin(w_border);
    delwin(w_main);
}

void CursesFrame::Refresh() {
    if (w_border != NULL) {
        wborder(w_border, '|', '|', '-', '-', '+', '+', '+', '+');
        //wborder(left_border, L'│', L'│', L'─', L'─', L'┌', L'┐', L'┘', L'└');

        wattron(w_border, A_BOLD);
        mvwprintw(w_border, 0, 1, header.c_str());
        wattroff(w_border, A_BOLD);

        wrefresh(w_border);
    }
    wrefresh(w_main);
}

void CursesFrame::PrintW(string str, int attr) {
    if (attr != 0) wattron(w_main, attr);
    wprintw(w_main, str.c_str());
    if (attr != 0) wattroff(w_main, attr);
}

void CursesFrame::MvPrintW(int x, int y, string str, int attr) {
    if (attr != 0) wattron(w_main, attr);
    mvwprintw(w_main, y, x, str.c_str());
    if (attr != 0) wattroff(w_main, attr);
}

void CursesFrame::Clear() {
    wclear(w_main);
}

int CursesFrame::UsableHeight() const {
    return w_main->_maxy;
}

int CursesFrame::UsableWidth() const {
    return w_main->_maxx;
}

void CursesFrame::SetFooter(string str) {
    footer = str;
}

void CursesFrame::SetHeader(string str) {
    header = str;
}
