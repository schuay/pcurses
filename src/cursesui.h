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

#include <vector>

#include "attributeinfo.h"

enum PaneEnum {
    PANE_LIST,
    PANE_QUEUE
};

class CursesListBox;
class CursesFrame;
class Package;
class State;

class CursesUi
{
public:
    static CursesUi &ui();

    /* Enable ncurses handling of the console. */
    void enable_curses(std::vector<Package *> *pkgs, std::vector<Package *> *queue);

    /* Disable ncurses handling of the console. */
    void disable_curses();

    /* Switches focus (if possible). */
    void switch_focus();

    /* Sets focus to the specified pane (if possible). */
    void set_focus(enum PaneEnum pane);

    /* Paints the screen, called at least once every mainloop iteration. */
    void update_display(const State &state);

    /* If a resize has been requested, reposition all windows and update the display. */
    void handle_resize(const State &state);

    inline CursesListBox *list() {
        return list_pane;
    }
    inline CursesListBox *queue() {
        return queue_pane;
    }
    inline CursesListBox *focused() {
        return focused_pane;
    }

private:
    /* Prevent class construction and copying. */
    CursesUi() { }
    CursesUi(const CursesUi &) { }

    /* Adjusts all pane sizes and positions to the current terminal size. */
    void resize();

    void print_help();
    void printinfosection(AttributeEnum attr, std::string text);

    /* Throws exception if terminal size is below a fixed limit. */
    void ensure_min_term_size(uint w, uint h) const;


    static CursesUi instance;

    CursesListBox *list_pane,
                  *queue_pane,
                  *focused_pane;

    CursesFrame *info_pane,
                *input_pane,
                *help_pane,
                *status_pane;
};

#endif // CURSESUI_H
