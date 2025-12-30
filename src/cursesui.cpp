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

#include <cassert>
#include <clocale>
#include <cstdlib>
#include <ncurses.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "cursesframe.h"
#include "curseslistbox.h"
#include "frameinfo.h"
#include "globals.h"
#include "package.h"
#include "pcursesexception.h"
#include "state.h"

using std::vector;

/* Static instance. */
CursesUi CursesUi::instance;

/* Resize signal handler. */
static volatile bool want_resize = false;
static void request_resize(int /* unused */)
{
    want_resize = true;
}

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

void CursesUi::switch_focus()
{
    set_focus((focused_pane == list_pane) ? PANE_QUEUE : PANE_LIST);
}

void CursesUi::set_focus(enum PaneEnum pane)
{
    enum PaneEnum p = pane;

    /* If the queue pane is empty it must not be focused. */
    if (queue_pane->empty()) {
        p = PANE_LIST;
    }

    switch (p) {
    case PANE_LIST:
        focused_pane = list_pane;
        break;
    case PANE_QUEUE:
        focused_pane = queue_pane;
        break;
    default:
        assert(0);
    }

    list_pane->setfocused(p == PANE_LIST);
    queue_pane->setfocused(p == PANE_QUEUE);
}

void CursesUi::resize()
{
    want_resize = false;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    ensure_min_term_size(w.ws_col, w.ws_row);

    endwin();
    refresh();

    CursesUi::ui().list_pane->reposition(w.ws_col, w.ws_row);
    CursesUi::ui().info_pane->reposition(w.ws_col, w.ws_row);
    CursesUi::ui().queue_pane->reposition(w.ws_col, w.ws_row);
    CursesUi::ui().status_pane->reposition(w.ws_col, w.ws_row);
    CursesUi::ui().input_pane->reposition(w.ws_col, w.ws_row);
    CursesUi::ui().help_pane->reposition(w.ws_col, w.ws_row);
}

void CursesUi::handle_resize(const State &state)
{
    if (!want_resize) {
        return;
    }

    resize();
    update_display(state);
}

void CursesUi::enable_curses(vector<Package *> *pkgs, vector<Package *> *queue)
{
    if (system("clear") == -1) {
        throw PcursesException("system() failed");
    }

    signal(SIGWINCH, request_resize);

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
    list_pane->setlist(pkgs);
    queue_pane->setlist(queue);
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

void CursesUi::printinfosection(AttributeEnum attr, string text)
{
    string caption = AttributeInfo::attrname(attr);
    char hllower = AttributeInfo::attrtochar(attr);
    char hlupper = toupper(hllower);
    bool hldone = false;
    int style;

    for (uint i = 0; i < caption.size(); i++) {
        if (!hldone && (caption[i] == hllower || caption[i] == hlupper)) {
            style = C_DEF;
            hldone = true;
        } else {
            style = C_DEF_HL2;
        }


        info_pane->printw(string(1, caption[i]), style);
    }
    info_pane->printw(": ", C_DEF_HL2);

    string txt = text + "\n";
    info_pane->printw(txt);
}

void CursesUi::update_display(const State &state)
{
    if (want_resize) {
        resize();
    }

    /* this runs **at least** once per loop iteration
       for example it can run more than once if we need to display
       a 'processing' message during filtering
     */

    if (state.mode == MODE_INPUT || state.mode == MODE_STANDARD) {
        Package *pkg;

        erase();
        list_pane->clear();
        info_pane->clear();
        status_pane->clear();
        input_pane->clear();
        queue_pane->clear();

        /* info pane */
        pkg = focused_pane->focusedpackage();
        if (pkg) {
            for (int i = 0; i < A_NONE; i++) {
                AttributeEnum attr = (AttributeEnum)i;
                string txt = pkg->getattr(attr);
                if (txt.length() != 0) {
                    printinfosection(attr, txt);
                }
            }
        }

        /* status bar */
        status_pane->mvprintw(1, 0, "Sorted by: ", C_INV_HL1);
        status_pane->printw(AttributeInfo::attrname(state.sortedby), C_INV);
        status_pane->printw(" Colored by: ", C_INV_HL1);
        status_pane->printw(AttributeInfo::attrname(state.coloredby), C_INV);
        status_pane->printw(" Filtered by: ", C_INV_HL1);
        status_pane->printw(((state.searchphrases.length() == 0)
                             ? "-" : state.searchphrases), C_INV);

        wnoutrefresh(stdscr);
        list_pane->refresh();
        queue_pane->refresh();
        info_pane->refresh();
        status_pane->refresh();

        if (state.mode == MODE_INPUT) {
            input_pane->printw(optostr(state.op) + state.inputbuf.getcontents());
            input_pane->move(state.inputbuf.getpos() + 1, 0);
            input_pane->refresh();
        }
    } else if (state.mode == MODE_HELP) {
        help_pane->clear();
        print_help();
        help_pane->refresh();
    }

    doupdate();
}

#define PRINTH(a, b) help_pane->printw(a, A_BOLD); help_pane->printw(b);
void CursesUi::print_help()
{
    PRINTH("esc: ", "cancel\n");
    PRINTH("q: ", "quit\n");
    PRINTH("1 to 0: ", "hotkeys (as configured in " APPLICATION_NAME ".conf)\n");
    PRINTH("!: ", "execute command, replacing %p with selected package names\n");
    PRINTH("@: ", "run the specified macro (as configured in " APPLICATION_NAME ".conf)\n");
    PRINTH("%: ", "run the specified control command (for example, %filter_clear)\n");
    PRINTH("r: ", "reload package info\n");
    PRINTH("/: ", "filter packages by specified fields (using regexp)\n");
    PRINTH("", "   note that filters can be chained.\n")
    PRINTH("n: ", "filter packages by name (using regexp)\n");
    PRINTH("c: ", "clear all package filters\n");
    PRINTH("C: ", "clear the package queue\n");
    PRINTH("?: ", "search packages\n");
    PRINTH(".: ", "sort packages by specified field\n");
    PRINTH(";: ", "colorcode packages by specified field\n");
    PRINTH("tab: ", "switch focus between list and queue panes\n");
    PRINTH("left/right arrows: ", "add/remove packages from the queue\n");
    PRINTH("up/down arrows, pg up/down, home/end: ", "navigation\n");
    PRINTH("up/down arrows (in input mode): ", "browse history\n");
    help_pane->printw("\n");
    help_pane->printw("configure macros, hotkeys and hooks in " APPLICATION_NAME ".conf\n");
}
#undef PRINTH
