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

#include "frameinfo.h"

using std::string;

CursesFrame::CursesFrame(FrameInfo *frameinfo)
    : overflowind("..."), w_main(NULL), w_border(NULL), focused(false), finfo(frameinfo)
{
    if (finfo->gethasborder()) {
        w_border = newwin(finfo->geth(), finfo->getw(), finfo->gety(), finfo->getx());
        w_main = newwin(finfo->geth() - 2, finfo->getw() - 2, finfo->gety() + 1, finfo->getx() + 1);
    } else {
        w_main = newwin(finfo->geth(), finfo->getw(), finfo->gety(), finfo->getx());
    }
    header = finfo->gettitle();
}

CursesFrame::~CursesFrame()
{
    if (w_border != NULL) {
        delwin(w_border);
    }
    delwin(w_main);
    delete finfo;
}

void CursesFrame::setbackground(chtype col)
{
    wbkgd(w_main, col | ' ');
}

void CursesFrame::move(int x, int y)
{
    wmove(w_main, y, x);
}

void CursesFrame::reposition(int termw, int termh)
{
    finfo->recalcinfo(termw, termh);

    if (finfo->gethasborder()) {
        wresize(w_border, finfo->geth(), finfo->getw());
        mvwin(w_border, finfo->gety(), finfo->getx());
        wresize(w_main, finfo->geth() - 2, finfo->getw() - 2);
        mvwin(w_main, finfo->gety() + 1, finfo->getx() + 1);
    } else {
        wresize(w_main, finfo->geth(), finfo->getw());
        mvwin(w_main, finfo->gety(), finfo->getx());
    }
}

string CursesFrame::fitstrtowin(string in, int x) const
{
    in = escapestring(in);
    int len = in.length();

    if (len == 0 || in[len - 1] != '\n') {
        return in;
    }

    /* avoid situations which gives us double line breaks through wrapping */

    int pos = (x == -1) ? getcurx(w_main) : x;

    if ((pos - 1 + len - 1) % usablewidth() != 0) {
        return in;
    }

    return in.substr(0, len - 1);
}
void CursesFrame::refresh()
{
    if (w_border != NULL) {
        box(w_border, ACS_VLINE, ACS_HLINE);

        int headercol = focused ? C_INV : C_DEF;
        wattron(w_border, A_BOLD | headercol);
        mvwprintw(w_border, 0, 1, "%s", header.c_str());
        wattroff(w_border, A_BOLD | headercol);

        mvwprintw(w_border, w_border->_maxy, 1, "%s", footer.c_str());

        wnoutrefresh(w_border);
    }
    wnoutrefresh(w_main);
}

void CursesFrame::printw(string str, int attr)
{
    if (attr != 0) {
        wattron(w_main, attr);
    }
    wprintw(w_main, "%s", fitstrtowin(str).c_str());
    if (attr != 0) {
        wattroff(w_main, attr);
    }
}

void CursesFrame::mvprintw(int x, int y, string str, int attr)
{
    if (attr != 0) {
        wattron(w_main, attr);
    }
    mvwprintw(w_main, y, x, "%s", fitstrtowin(str, x).c_str());
    if (attr != 0) {
        wattroff(w_main, attr);
    }
}

string CursesFrame::escapestring(string str) const
{
    string ret = string(str);
    size_t pos = 0;
    while ((pos = ret.find("%", pos)) != string::npos) {
        ret.replace(pos, 1, "%%");
        pos++;
        pos++;
    }
    return ret;
}

void CursesFrame::clear()
{
    werase(w_main);
}

int CursesFrame::usableheight() const
{
    return w_main->_maxy;
}

int CursesFrame::usablewidth() const
{
    return w_main->_maxx;
}

void CursesFrame::setfooter(string str)
{
    if (str.length() > (uint)usablewidth()) {
        str = str.substr(0, usablewidth() - overflowind.length() + 1) + overflowind;
    }

    footer = str;
}

void CursesFrame::setheader(string str)
{
    if (str.length() + finfo->gettitle().length() > (uint)usablewidth()) {
        int newlen = usablewidth() - finfo->gettitle().length() - overflowind.length();
        str = str.substr(0, usablewidth() - newlen) + overflowind;
    }

    header = finfo->gettitle() + " " + str;
}
