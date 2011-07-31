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

#include "history.h"

History::History()
{
    reset();
}

void History::reset() {
    pos = history.begin();
    inhis = false;
}

string History::moveforward() {
    if (pos != history.begin()) pos--;
    return *pos;
}

string History::moveback() {
    if (!inhis) {
        inhis = true;
        return *pos;
    }

    list<string>::iterator it = pos;
    if (pos != history.end() && ++it != history.end()) pos++;
    return *pos;
}

void History::add(string s) {

    if (s.length() == 0)
        return;

    history.remove(s);
    history.push_front(s);
}

bool History::empty() const {
    return history.empty();
}
