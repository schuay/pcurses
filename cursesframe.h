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

#ifndef CURSESFRAME_H
#define CURSESFRAME_H

#include <ncurses.h>
#include <string>

using std::string;

class CursesFrame
{
public:
    CursesFrame(int w, int h, int x, int y, bool hasborder);
    virtual ~CursesFrame();

    void SetHeader(string str);
    void SetFooter(string str);
    virtual void Refresh();
    void PrintW(string str, int attr = 0);
    void MvPrintW(int x, int y, string str, int attr = 0);
    void Clear();

    int UsableHeight() const;
    int UsableWidth() const;
protected:
    WINDOW
            *w_main,
            *w_border;
    string
            header,
            footer;
};

#endif // CURSESFRAME_H
