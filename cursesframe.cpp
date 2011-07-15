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
    : overflowind("..."), w_main(NULL), w_border(NULL)
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

void CursesFrame::DoUpdate() {
    doupdate();
}

void CursesFrame::SetBackground(chtype col) {
    wbkgd(w_main, col | ' ');
}

string CursesFrame::FitStrToWin(string in, int x) const {
    in = EscapeString(in);
    int len = in.length();

    if (len == 0 || in[len-1] != '\n')
        return in;

    /* avoid situations which gives us double line breaks through wrapping */

    int pos = (x == -1) ? getcurx(w_main) : x;

    if ((pos - 1 + len - 1) % UsableWidth() != 0)
        return in;

    return in.substr(0, len-1);
}
void CursesFrame::Refresh() {
    if (w_border != NULL) {
        box(w_border, ACS_VLINE, ACS_HLINE);

        wattron(w_border, A_BOLD);
        mvwprintw(w_border, 0, 1, header.c_str());
        wattroff(w_border, A_BOLD);

        mvwprintw(w_border, w_border->_maxy, 1, footer.c_str());

        wnoutrefresh(w_border);
    }
    wnoutrefresh(w_main);
}

void CursesFrame::PrintW(string str, int attr) {
    if (attr != 0) wattron(w_main, attr);
    wprintw(w_main, FitStrToWin(str).c_str());
    if (attr != 0) wattroff(w_main, attr);
}

void CursesFrame::MvPrintW(int x, int y, string str, int attr) {
    if (attr != 0) wattron(w_main, attr);
    mvwprintw(w_main, y, x, FitStrToWin(str, x).c_str());
    if (attr != 0) wattroff(w_main, attr);
}

string CursesFrame::EscapeString(string str) const {
    string ret = string(str);
    size_t pos = 0;
    while ((pos = ret.find("%", pos)) != string::npos) {
        ret.replace(pos, 1, "%%");
        pos++;
        pos++;
    }
    return ret;
}

void CursesFrame::Clear() {
    werase(w_main);
}

int CursesFrame::UsableHeight() const {
    return w_main->_maxy;
}

int CursesFrame::UsableWidth() const {
    return w_main->_maxx;
}

void CursesFrame::SetFooter(string str) {
    if (str.length() > (unsigned int)UsableWidth()) {
        str = str.substr(0, UsableWidth() - overflowind.length() + 1) + overflowind;
    }

    footer = str;
}

void CursesFrame::SetHeader(string str) {
    if (str.length() > (unsigned int)UsableWidth()) {
        str = str.substr(0, UsableWidth() - overflowind.length() + 1) + overflowind;
    }

    header = str;
}
