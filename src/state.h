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

#ifndef STATE_H
#define STATE_H

#include <string>

#include "inputbuffer.h"
#include "attributeinfo.h"

enum ModeEnum {
    MODE_STANDARD,
    MODE_INPUT,
    MODE_HELP
};

enum FilterOperationEnum {
    OP_SEARCH,
    OP_FILTER,
    OP_SORT,
    OP_COLORCODE,
    OP_EXEC,
    OP_MACRO,
    OP_NONE
};

enum ControlOperationEnum {
    CTRL_SCROLL_UP = 0,
    CTRL_SCROLL_DOWN,
    CTRL_SCROLL_HOME,
    CTRL_SCROLL_END,
    CTRL_SCROLL_PAGEUP,
    CTRL_SCROLL_PAGEDOWN,
    CTRL_SWITCH_FOCUS,
    CTRL_QUEUE_PUSH,
    CTRL_QUEUE_POP,
    CTRL_QUEUE_CLEAR,
    CTRL_HELP,
    CTRL_QUIT,
    CTRL_RELOAD,
    CTRL_FILTER_CLEAR,
};

struct State {
    State();

    ModeEnum mode;
    std::string searchphrases;
    InputBuffer inputbuf;
    AttributeEnum sortedby,
                  coloredby;
    FilterOperationEnum op;
};

std::string optostr(FilterOperationEnum o);
FilterOperationEnum strtoopt(std::string str);

#endif // STATE_H
