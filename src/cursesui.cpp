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

#include "cursesui.h"

/* Static instance. */
CursesUi CursesUi::instance;

CursesUi &CursesUi::ui()
{
    return instance;
}

void CursesUi::ensure_min_term_size(uint w, uint h) const
{
    const uint minw = 60;
    const uint minh = 20;

    if (w < minw || h < minh) {
        throw PcursesException("Window size is below required minimum");
    }
}

void CursesUi::set_focus(enum PaneEnum pane)
{
    switch (pane)
    {
    case PANE_LIST: focused_pane = list_pane; break;
    case PANE_QUEUE: focused_pane = queue_pane; break;
    default: assert(0);
    }

    list_pane->setfocused(pane == PANE_LIST);
    queue_pane->setfocused(pane == PANE_QUEUE);
}

void CursesUi::enable_curses()
{
    if (system("clear") == -1) {
        throw PcursesException("system() failed");
    }

    setlocale(LC_ALL, "");

    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    /* getch() is our loop speed limiter. we use the nonblocking version
      to handle window resizes without waiting for the next key event, but
      use a timeout of 50 ms to limit the cpu usage to acceptable levels.
      50 ms is just an initial value and is subject to change depending on how
      well it works */
    timeout(50);

    /* target ist archlinux so we know a proper ncurses will be used.
       otherwise we would need to conditionally include this using
       NCURSES_VERSION */
    use_default_colors();

    ensure_min_term_size(COLS, LINES);

    init_pair(5, -1, -1);                   /* default (pane BG) */
    init_pair(2, COLOR_GREEN, -1);          /* default highlight 1 */
    init_pair(3, COLOR_CYAN, -1);           /* default highlight 2 */
    init_pair(6, COLOR_BLUE, -1);
    init_pair(7, COLOR_MAGENTA, -1);
    init_pair(8, COLOR_RED, -1);
    init_pair(9, COLOR_YELLOW, -1);
    init_pair(1, COLOR_BLACK, COLOR_WHITE); /* inverted (status bar BG) */
    init_pair(4, COLOR_BLUE, COLOR_WHITE);  /* inverted highlight 1 */

    list_pane = new CursesListBox(new FrameInfo(FE_LIST, COLS, LINES));
    info_pane = new CursesFrame(new FrameInfo(FE_INFO, COLS, LINES));
    queue_pane = new CursesListBox(new FrameInfo(FE_QUEUE, COLS, LINES));
    status_pane = new CursesFrame(new FrameInfo(FE_STATUS, COLS, LINES));
    input_pane = new CursesFrame(new FrameInfo(FE_INPUT, COLS, LINES));
    help_pane = new CursesFrame(new FrameInfo(FE_HELP, COLS, LINES));

    list_pane->setbackground(C_DEF);
    info_pane->setbackground(C_DEF);
    queue_pane->setbackground(C_DEF);
    status_pane->setbackground(C_INV);
    input_pane->setbackground(C_DEF);
    help_pane->setbackground(C_DEF);

    set_focus(PANE_LIST);
//    list_pane->setlist(&filteredpackages);
//    queue_pane->setlist(&opqueue);
}

void CursesUi::set_package_list(vector<Package *> *list)
{
    list_pane->setlist(list);
}

void CursesUi::set_queue_list(vector<Package *> *list)
{
    queue_pane->setlist(list);
}

void CursesUi::disable_curses()
{
    delete list_pane;
    delete queue_pane;
    delete info_pane;
    delete status_pane;
    delete input_pane;
    delete help_pane;

    nocbreak();
    curs_set(1);
    echo();

    endwin();

    if (system("clear") == -1) {
        throw PcursesException("system() failed");
    }
}
