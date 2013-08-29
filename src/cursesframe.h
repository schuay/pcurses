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

#ifndef CURSESFRAME_H
#define CURSESFRAME_H

#include <ncurses.h>
#include <string>

#define C_DEF (COLOR_PAIR(5))
#define C_DEF_HL1 (COLOR_PAIR(2))
#define C_DEF_HL2 (COLOR_PAIR(3))
#define C_DEF_HL3 (COLOR_PAIR(6))
#define C_DEF_HL4 (COLOR_PAIR(7))
#define C_DEF_HL5 (COLOR_PAIR(8))
#define C_DEF_HL6 (COLOR_PAIR(9))
#define C_INV (COLOR_PAIR(1))
#define C_INV_HL1 (COLOR_PAIR(4))

class FrameInfo;

class CursesFrame
{
public:
    CursesFrame(FrameInfo *frameinfo);
    virtual ~CursesFrame();

    void reposition(int termw, int termh);
    void setbackground(chtype col);
    void setheader(std::string str);
    void setfooter(std::string str);
    virtual void refresh();
    void printw(std::string str, int attr = 0);
    void mvprintw(int x, int y, std::string str, int attr = 0);
    void move(int x, int y);
    void clear();
    void setfocused(bool b) {
        focused = b;
    }

    int usableheight() const;
    int usablewidth() const;

protected:

    std::string fitstrtowin(std::string in, int x = -1) const;
    std::string escapestring(std::string str) const;

    const std::string overflowind;

    WINDOW *w_main,
           *w_border;
    std::string header,
        footer;

    bool focused;

    FrameInfo *finfo;
};

#endif // CURSESFRAME_H
