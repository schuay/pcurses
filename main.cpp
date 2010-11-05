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

#include "program.h"

bool cmp(const Package *a, const Package *b) {
    return a->name() < b->name();
}
bool pkgsearch(const Package *a, const std::string needle, const std::string op) {
    bool found = false;

    if (op == "n/" || op == "/") {
        found = found || a->name().find(needle) != std::string::npos;
    }
    if (op == "d/" || op == "/") {
        found = found || a->desc().find(needle) != std::string::npos;
    }
    return !found;
}

int main() {

    Program *p = new Program();

    p->Init();
    p->MainLoop();

    delete p;

    return 0;
}
