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

#ifndef CURSESUI_H
#define CURSESUI_H

#include <ncurses.h>
#include <vector>

#include "cursesframe.h"
#include "curseslistbox.h"

using std::vector;

enum PaneEnum {
    PANE_LIST,
    PANE_QUEUE
};

class CursesUi
{
public:
    static CursesUi &ui();

    /* Enable ncurses handling of the console. */
    void enable_curses();

    /* Disable ncurses handling of the console. */
    void disable_curses();

    /* Throws exception if terminal size is below a fixed limit. */
    void ensure_min_term_size(uint w, uint h) const;

    /* Sets focus to the specified pane. */
    void set_focus(enum PaneEnum pane);

    void set_package_list(vector<Package *> *list);
    void set_queue_list(vector<Package *> *list);

    /* TODO: set these private. */
    CursesListBox *list_pane,
                  *queue_pane,
                  *focused_pane;
    CursesFrame *info_pane,
                *input_pane,
                *help_pane,
                *status_pane;

private:
    static CursesUi instance;
};

#endif // CURSESUI_H
