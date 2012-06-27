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

#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <assert.h>
#include <string>
#include <sys/types.h>

using std::string;

enum FrameEnum {
    FE_LIST,
    FE_INFO,
    FE_QUEUE,
    FE_STATUS,
    FE_INPUT,
    FE_HELP
};

class FrameInfo
{
public:

    FrameInfo(FrameEnum t, uint termw, uint termh);
    void recalcinfo(uint termw, uint termh);

    uint getx() const { return x; }
    uint gety() const { return y; }
    uint getw() const { return w; }
    uint geth() const { return h; }

    bool gethasborder() const { return hasborder; }

    string gettitle() const { return title; }

private:

    uint w, h, x, y;

    bool hasborder;

    string title;

    const FrameEnum type;
};

#endif // FRAMEINFO_H
