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

using std::string;

#define C_DEF (COLOR_PAIR(5))
#define C_DEF_HL1 (COLOR_PAIR(2))
#define C_DEF_HL2 (COLOR_PAIR(3))
#define C_DEF_HL3 (COLOR_PAIR(6))
#define C_DEF_HL4 (COLOR_PAIR(7))
#define C_DEF_HL5 (COLOR_PAIR(8))
#define C_DEF_HL6 (COLOR_PAIR(9))
#define C_INV (COLOR_PAIR(1))
#define C_INV_HL1 (COLOR_PAIR(4))

class CursesFrame
{
public:
    CursesFrame(int w, int h, int x, int y, bool hasborder);
    virtual ~CursesFrame();

    void SetBackground(chtype col);
    void SetHeader(string str);
    void SetFooter(string str);
    virtual void Refresh();
    void PrintW(string str, int attr = 0);
    void MvPrintW(int x, int y, string str, int attr = 0);
    void Clear();

    int UsableHeight() const;
    int UsableWidth() const;

    static void DoUpdate();

protected:

    string FitStrToWin(string in, int x = -1) const;

    const string
            overflowind;

    WINDOW
            *w_main,
            *w_border;
    string
            header,
            footer;
};

#endif // CURSESFRAME_H
