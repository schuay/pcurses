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

#ifndef INPUTBUFFER_H
#define INPUTBUFFER_H

#include <string>

using std::string;

class InputBuffer
{
public:
    InputBuffer();

    string getcontents() const { return contents; }
    size_t getpos() const { return pos; }

    void set(string str);
    void insert(const char c);

    void backspace();
    void del();
    void clear() { contents = ""; pos = 0; }

    void movestart();
    void moveend();
    void moveleft();
    void moveright();

private:
    string contents;
    size_t pos;
};

#endif // INPUTBUFFER_H
