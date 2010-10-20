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

#include "package.h"
#include "alpmexception.h"
#include "cursesframe.h"
#include "curseslistbox.h"

typedef struct __pmdb_t pmdb_t;
typedef struct __alpm_list_t alpm_list_t;

CursesListBox
        *list_pane;
CursesFrame
        *info_pane;

std::vector<Package*>
        packages,
        filteredpackages;

pmdb_t
        *localdb;

std::string
        searchphrase,
        op = "";

enum ModeEnum {
    MODE_STANDARD,
    MODE_INPUT,
};
ModeEnum mode;

void init_alpm() {
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
}

void init_curses() {

    setlocale(LC_ALL, "");

    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    const int lwidth = 30;
    list_pane = new CursesListBox(lwidth, LINES - 1, 0, 0, true);
    info_pane = new CursesFrame(COLS - lwidth + 1, LINES - 1, 0, lwidth - 1, true);

    list_pane->SetHeader("Pkg List");
    info_pane->SetHeader("Pkg Info");
}
void deinit_curses() {
    delete list_pane;
    delete info_pane;

    nocbreak();
    curs_set(1);
    echo();

    endwin();
}

void printinfosection(std::string header, std::string text) {

    std::string hdr = header;
    std::string txt = text + "\n";

    info_pane->PrintW(hdr, COLOR_PAIR(3));
    info_pane->PrintW(txt);
}
void updatedisplay() {

    Package *pkg;

    clear();
    list_pane->Clear();
    info_pane->Clear();

    if ((unsigned int)(list_pane->FocusedIndex()) < filteredpackages.size()) {
        pkg = filteredpackages[list_pane->FocusedIndex()];
        printinfosection("name: ", pkg->name());
        printinfosection("version: ", pkg->version());
        printinfosection("repo: ", pkg->dbname());
        printinfosection("builddate: ", pkg->builddate());
        printinfosection("desc: ", pkg->desc());
    }

    std::string label = op + searchphrase;
    if (mode == MODE_INPUT)
        mvprintw(LINES - 1, 0, label.c_str());

    refresh();
    list_pane->Refresh();
    info_pane->Refresh();
}

//void cb_log(pmloglevel_t/* level*/, char *fmt, va_list args)
//{
//  if(!fmt) {
//    return;
//  }
//  printf(fmt, args);
//}

bool cmp(const Package *a, const Package *b) {
    return a->name() < b->name();
}
bool eqname(const Package *a, const std::string name) {
    return a->name() == name;
}
bool pkgsearch(const Package *a, const std::string needle) {
    bool found = false;

    if (op == "n/" || op == "/") {
        found = found || a->name().find(needle) != std::string::npos;
    }
    if (op == "d/" || op == "/") {
        found = found || a->desc().find(needle) != std::string::npos;
    }
    return !found;
}

void filterpackages(std::string searchphrase) {

    filteredpackages = packages;

    if (searchphrase.length() == 0)
        return;

    for (std::vector<Package*>::iterator it = std::find_if(filteredpackages.begin(),
                                                 filteredpackages.end(),
                                                 std::bind2nd(std::ptr_fun(pkgsearch), searchphrase));
        it != filteredpackages.end();
        it = std::find_if(filteredpackages.begin(),
                          filteredpackages.end(),
                          std::bind2nd(std::ptr_fun(pkgsearch), searchphrase)))
        filteredpackages.erase(it);
}

int main() {

    mode = MODE_STANDARD;

    std::cout << "Reading package dbs, please wait..." << std::endl;

    init_alpm();

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

    system("clear");
    init_curses();
    list_pane->SetList(&filteredpackages);
    updatedisplay();
    int ch = getch();
    while (mode == MODE_INPUT || ch != 'q') {
        if (mode == MODE_STANDARD) {
            switch (ch) {
            case KEY_UP:
                list_pane->Move(-1);
                break;
            case KEY_DOWN:
                list_pane->Move(1);
                break;
            case 262:   /* pos 1 */
                list_pane->MoveAbs(0);
                break;
            case 360:   /* end */
                list_pane->MoveAbs(filteredpackages.size() - 1);
                break;
            case 339:   /* page up */
                list_pane->Move(list_pane->UsableHeight() * -1);
                break;
            case 338:   /* page down */
                list_pane->Move(list_pane->UsableHeight());
                break;
            case 'n':
                op = "n";
                break;
            case 'd':
                op = "d";
                break;
            case '/':
                mode = MODE_INPUT;
                searchphrase = "";
                op += "/";
                break;
            default:
                break;
            }
        }
        else if (mode == MODE_INPUT) {
            switch (ch) {
            case 27:    /* ESC */
                mode = MODE_STANDARD;
                op = "";
                break;
            case 10:    /* ENTER */
                mode = MODE_STANDARD;
                filterpackages(searchphrase);
                list_pane->SetList(&filteredpackages);
                list_pane->SetHeader("Pkg List (filter: " + op + searchphrase + " )");
                op = "";
                break;
            default:
                searchphrase += ch;
                break;
            }
        }

        updatedisplay();
        ch = getch();
    }
    deinit_curses();

    for (unsigned int i = 0; i < filteredpackages.size(); i++)
        delete filteredpackages[i];

    if (alpm_release() != 0)
        throw AlpmException("failed to deinitialize alpm library");

    return 0;
}
