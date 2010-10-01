/***************************************************************************

    Copyright 2010 Jakob Gruber

    This file is part of dungeons.

    dungeons is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    dungeons is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with dungeons.  If not, see <http://www.gnu.org/licenses/>.

 ***************************************************************************/

#include <algorithm>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <stdarg.h>

#include "lib/package.h"
#include "lib/alpmexception.h"

typedef struct __pmdb_t pmdb_t;
typedef struct __pmpkg_t pmpkg_t;
typedef struct __alpm_list_t alpm_list_t;

WINDOW
        *left_border,
        *right_border,
        *list_pane,
        *info_pane;

std::vector<Package*>
        packages,
        filteredpackages;
pmdb_t
        *localdb;

std::string
        searchphrase,
        op;

int
        windowpos = 0,
        cursorpos = 0;

enum ModeEnum {
    MODE_STANDARD,
    MODE_INPUT,
};
ModeEnum mode;

void init() {

    setlocale(LC_ALL, "");

    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    int
            lwidth;

    lwidth = 30;//COLS / 2;
    left_border = newwin(LINES, lwidth, 0, 0);
    list_pane = newwin(LINES - 2, lwidth - 2, 1, 1);
    right_border = newwin(LINES, COLS - lwidth + 1, 0, lwidth - 1);
    info_pane = newwin(LINES - 2, COLS - lwidth + 1 - 2, 1, lwidth - 1 + 1);

//    wborder(left_border, L'│', L'│', L'─', L'─', L'┌', L'┐', L'┘', L'└');
//    wborder(right_border, L'│', L'│', L'─', L'─', L'┌', L'┐', L'┘', L'└');

}

void deinit() {
    delwin(list_pane);
    delwin(info_pane);
    endwin();
}

void printinfosection(std::string header, std::string text) {

    std::string hdr = header + "\n";
    std::string txt = text + "\n\n";

    wattron(info_pane,A_BOLD);
    wprintw(info_pane, hdr.c_str());
    wattroff(info_pane,A_BOLD);

    wprintw(info_pane, txt.c_str());
}
void updatedisplay(int windowpos, int cursorpos) {

    Package *pkg;

    wclear(list_pane);
    wclear(info_pane);

    for (int i = 0; i <= list_pane->_maxy; i++) {
        if ((unsigned int)(windowpos + i + 1) > filteredpackages.size())
            break;

        pkg = filteredpackages[windowpos + i];
        if (pkg->installed())
            wattron(list_pane, COLOR_PAIR(2));
        if(pkg->needsupdate())
            wattron(list_pane, COLOR_PAIR(3));

        if (i == cursorpos) wattron(list_pane,A_STANDOUT);

        mvwprintw(list_pane, i, 0, pkg->name().substr(0, list_pane->_maxx + 1).c_str());

        wattroff(list_pane, A_STANDOUT | COLOR_PAIR(2) | COLOR_PAIR(3));
    }

    if (windowpos + cursorpos < filteredpackages.size()) {
        pkg = filteredpackages[windowpos + cursorpos];
        printinfosection("name:", pkg->name());
        printinfosection("version:", pkg->version());
        printinfosection("repo:", pkg->dbname());
        printinfosection("builddate:", pkg->builddate());
        printinfosection("desc:", pkg->desc());
    }

    wborder(left_border, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(right_border, '|', '|', '-', '-', '+', '+', '+', '+');

    std::string label = op + searchphrase;
    if (mode == MODE_INPUT)
        mvwprintw(left_border, LINES - 1, 1, label.c_str());

    refresh();
    wrefresh(left_border);
    wrefresh(right_border);
    wrefresh(list_pane);
    wrefresh(info_pane);
}

void cb_log(pmloglevel_t/* level*/, char *fmt, va_list args)
{
  if(!fmt) {
    return;
  }

  printf(fmt, args);
}

bool cmp(const Package *a, const Package *b) {
    return a->name() < b->name();
}
bool eqname(const Package *a, const std::string name) {
    return a->name() == name;
}
bool cmpname(const Package *a, const std::string name) {
    return a->name().find(name) == std::string::npos &&
           a->desc().find(name) == std::string::npos;
}

bool isinbounds(int pos) {
    if (pos < 0)
        return false;
    if ((unsigned int)(pos) >= filteredpackages.size())
        return false;
    return true;
}
void mvfocus(int step) {
    if (!isinbounds(windowpos + cursorpos + step))
        return;
    else if (cursorpos + step >= 0 && cursorpos + step <= list_pane->_maxy)
        cursorpos += step;
    else if (windowpos + step >= 0 && windowpos + step <= filteredpackages.size())
        windowpos += step;
}
void updatefocus() {
    if ((unsigned int)(windowpos + cursorpos) >= filteredpackages.size()) {
        if ((int)filteredpackages.size() - list_pane->_maxy < 0)
            windowpos = 0;
        else
            windowpos = (int)filteredpackages.size() - list_pane->_maxy;

        cursorpos = filteredpackages.size() - windowpos - 1;
    }
}

void filterpackages(std::string searchphrase) {

    filteredpackages = packages;

    if (searchphrase.length() == 0)
        return;

    for (std::vector<Package*>::iterator it = std::find_if(filteredpackages.begin(),
                                                 filteredpackages.end(),
                                                 std::bind2nd(std::ptr_fun(cmpname), searchphrase));
        it != filteredpackages.end();
        it = std::find_if(filteredpackages.begin(),
                          filteredpackages.end(),
                          std::bind2nd(std::ptr_fun(cmpname), searchphrase)))
        filteredpackages.erase(it);
}

int main() {

    mode = MODE_STANDARD;

    if (alpm_initialize() != 0)
        throw AlpmException("failed to initialize alpm library");

//    alpm_option_set_logcb(cb_log);
    alpm_option_set_dbpath("/var/lib/pacman");

    alpm_db_register_local();

    alpm_db_register_sync("testing");
    alpm_db_register_sync("community");
    alpm_db_register_sync("extra");
    alpm_db_register_sync("core");

    localdb = alpm_option_get_localdb();

    alpm_list_t *dbs = alpm_list_copy(alpm_option_get_syncdbs());
    dbs = alpm_list_add(dbs, localdb);
    for ( ; dbs; dbs = alpm_list_next(dbs)) {
        pmdb_t *db = (pmdb_t*)alpm_list_getdata(dbs);
        for (alpm_list_t *pkgs = alpm_db_get_pkgcache(db); pkgs; pkgs = alpm_list_next(pkgs)) {
            pmpkg_t *pkg = (pmpkg_t*)alpm_list_getdata(pkgs);
            Package *p = new Package(pkg);
            Package *parray[] = { p };
            if (!std::includes(packages.begin(), packages.end(),
                              parray, parray + 1,
                              cmp)) {
                packages.push_back(p);
            }
            else
                delete p;
        }
        std::sort(packages.begin(), packages.end(), cmp);
    }
    alpm_list_free(dbs);

    filteredpackages = packages;

    init();
    updatedisplay(0, 0);
    int ch = getch();
    while (mode == MODE_INPUT || ch != 'q') {
        if (mode == MODE_STANDARD) {
            switch (ch) {
            case KEY_UP:
                mvfocus(-1);
                break;
            case KEY_DOWN:
                mvfocus(1);
                break;
            case 339:   /* page up */
                mvfocus(list_pane->_maxy * -1);
                break;
            case 338:   /* page down */
                mvfocus(list_pane->_maxy);
                break;
            case '/':
                mode = MODE_INPUT;
                searchphrase = "";
                op = "/";
                break;
            default:
                break;
            }
        }
        else if (mode == MODE_INPUT) {
            switch (ch) {
            case 27:    /* ESC */
                mode = MODE_STANDARD;
                break;
            case 10:    /* ENTER */
                mode = MODE_STANDARD;
                filterpackages(searchphrase);
                updatefocus();
                break;
            default:
                searchphrase += ch;
                break;
            }
        }

        updatedisplay(windowpos, cursorpos);
        ch = getch();
    }
    deinit();

    for (unsigned int i = 0; i < filteredpackages.size(); i++)
        delete filteredpackages[i];

    if (alpm_release() != 0)
        throw AlpmException("failed to deinitialize alpm library");

    return 0;
}
