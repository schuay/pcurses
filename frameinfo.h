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

    FrameInfo(FrameEnum t, unsigned int termw, unsigned int termh);
    void recalcinfo(unsigned int termw, unsigned int termh);

    unsigned int getx() const { return x; }
    unsigned int gety() const { return y; }
    unsigned int getw() const { return w; }
    unsigned int geth() const { return h; }
    bool gethasborder() const { return hasborder; }

private:

    unsigned int w, h, x, y;
    bool hasborder;

    const FrameEnum type;
};

#endif // FRAMEINFO_H
