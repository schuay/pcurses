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

#include "inputbuffer.h"

InputBuffer::InputBuffer()
{
    pos = 0;
    clear();
}

void InputBuffer::moveleft() {
    if (pos > 0) {
        pos--;
    }
}

void InputBuffer::moveright() {
    if (pos < contents.length()) {
        pos++;
    }
}

void InputBuffer::backspace() {
    if (pos > 0) {
        contents.erase(pos - 1, 1);
        moveleft();
    }
}

void InputBuffer::del() {
    if (pos < contents.length()) {
        contents.erase(pos, 1);
    }
}

void InputBuffer::insert(const char c) {
    contents.insert(pos, 1, c);
    pos++;
}

void InputBuffer::set(string str) {
    contents = str;
    pos = contents.length();
}
