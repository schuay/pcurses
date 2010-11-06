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

Program::Program()
{
    quit = false;
    op = "";
    rightpane = RPE_INFO;
    mode = MODE_STANDARD;
}
Program::~Program() {
    deinit_curses();

    for (unsigned int i = 0; i < filteredpackages.size(); i++)
        delete filteredpackages[i];

    if (alpm_release() != 0)
        throw AlpmException("failed to deinitialize alpm library");
}

void Program::Init() {

    std::cout << "Reading package dbs, please wait..." << std::endl;

    /* initialize libalpm */
    init_alpm();

    /* create our package list */
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
                              std::ptr_fun(&Package::cmp))) {
                packages.push_back(p);
            }
            else
                delete p;
        }
        std::sort(packages.begin(), packages.end(), std::ptr_fun(&Package::cmp));
    }
    alpm_list_free(dbs);

    filteredpackages = packages;

    /* initialize curses */
    system("clear");
    init_curses();
    focused_pane = list_pane;
    list_pane->SetList(&filteredpackages);
    queue_pane->SetList(&opqueue);
    updatedisplay();
}

void Program::MainLoop() {
    int ch;
    while (!quit) {
        ch = getch();

        if (mode == MODE_STANDARD) {
            switch (ch) {
            case KEY_UP:
                focused_pane->Move(-1);
                break;
            case KEY_DOWN:
                focused_pane->Move(1);
                break;
            case KEY_HOME:
                focused_pane->MoveAbs(0);
                break;
            case KEY_END:
                focused_pane->MoveAbs(filteredpackages.size() - 1);
                break;
            case KEY_PPAGE:   /* page up */
                focused_pane->Move(list_pane->UsableHeight() * -1);
                break;
            case KEY_NPAGE:   /* page down */
                focused_pane->Move(list_pane->UsableHeight());
                break;
            case KEY_TAB:
                focused_pane = (focused_pane == list_pane) ?
                               queue_pane : list_pane;
                break;
            case KEY_SPACE:
                rightpane = (rightpane == RPE_INFO) ? RPE_QUEUE : RPE_INFO;
                break;
            case KEY_RIGHT:
                if (focused_pane != list_pane) break;
                if (filteredpackages.size() == 0) break;
                opqueue.push_back(filteredpackages[list_pane->FocusedIndex()]);
                break;
            case KEY_LEFT:
                /* TODO */
                break;
            case 'h':
                mode = MODE_HELP;
                break;
            case 'q':
                quit = true;
                break;
            case 'n':
            case 'd':
                op = ch;
            case '/':
                mode = MODE_INPUT;
                searchphrase = "";
                op += "/";
                break;
            default:
                break;
            }
        } else if (mode == MODE_INPUT) {
            switch (ch) {
            case KEY_ESC:
                mode = MODE_STANDARD;
                op = "";
                break;
            case KEY_RETURN:
                mode = MODE_STANDARD;
                filterpackages(searchphrase);
                list_pane->SetList(&filteredpackages);
                list_pane->SetHeader("Pkg List (filter: " + op + searchphrase + " )");
                op = "";
                break;
            case KEY_BACKSPACE:
            case KEY_KONSOLEBACKSPACE:
                if (searchphrase.length() > 0)
                    searchphrase = searchphrase.substr(0, searchphrase.length() - 1);
                break;
            default:
                searchphrase += ch;
                break;
            }
        } else if (mode == MODE_HELP) {
            switch (ch) {
            case KEY_ESC:
            case KEY_RETURN:
            case KEY_SPACE:
                mode = MODE_STANDARD;
                break;
            default:
                break;
            }
        }

        updatedisplay();
    }
}

#define PRINTH(a, b) help_pane->PrintW(a, A_BOLD); help_pane->PrintW(b);
void Program::print_help() {
    help_pane->PrintW("COMMANDS\n", A_BOLD);
    help_pane->PrintW("\n");
    PRINTH("ESC: ", "cancel\n");
    PRINTH("q: ", "quit\n");
    PRINTH("/: ", "filter packages by desc and name (using regexp)\n");
    PRINTH("n: ", "filter packages by name (using regexp)\n");
    PRINTH("arrows, pg up/down, home/end: ", "navigation\n");
    PRINTH("return: ", "exit help\n");
    help_pane->PrintW("\n");
    help_pane->PrintW("\n");
    help_pane->PrintW("\n");
    help_pane->PrintW("\n");
    help_pane->PrintW("\n");
    help_pane->PrintW("\n");
}

void Program::init_alpm() {
    if (alpm_initialize() != 0)
        throw AlpmException("failed to initialize alpm library");

    Config conf;
    conf.parse();

    alpm_option_set_dbpath(conf.getDBPath().c_str());
    alpm_option_set_logfile(conf.getLogFile().c_str());
    alpm_option_set_root(conf.getRootDir().c_str());

    vector<string> repos = conf.getRepos();
    for (unsigned int i = 0; i < repos.size(); i++)
        alpm_db_register_sync(repos[i].c_str());

    alpm_db_register_local();
    localdb = alpm_option_get_localdb();
}

void Program::init_curses() {

    setlocale(LC_ALL, "");

    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);

    const int lwidth = 30;
    list_pane = new CursesListBox(lwidth, LINES, 0, 0, true);
    info_pane = new CursesFrame(COLS - lwidth + 1, LINES, 0, lwidth - 1, true);
    queue_pane = new CursesListBox(COLS - lwidth + 1, LINES, 0, lwidth - 1, true);
    input_pane = new CursesFrame(COLS, 3, LINES - 2, 0, true);
    help_pane = new CursesFrame(COLS - 10, LINES - 10, 5, 5, true);

    list_pane->SetHeader("Pkg List");
    info_pane->SetHeader("Pkg Info");
    info_pane->SetFooter("Press h for help");
    queue_pane->SetHeader("Pkg Queue");
    input_pane->SetHeader("Input");
    help_pane->SetHeader("Help");
}
void Program::deinit_curses() {
    delete list_pane;
    delete queue_pane;
    delete info_pane;
    delete input_pane;
    delete help_pane;

    nocbreak();
    curs_set(1);
    echo();

    endwin();
}

void Program::printinfosection(std::string header, std::string text) {

    std::string hdr = header;
    std::string txt = text + "\n";

    info_pane->PrintW(hdr, COLOR_PAIR(3));
    info_pane->PrintW(txt);
}
void Program::updatedisplay() {

    if (mode == MODE_INPUT || mode == MODE_STANDARD) {
        Package *pkg;

        clear();
        list_pane->Clear();
        info_pane->Clear();
        input_pane->Clear();
        queue_pane->Clear();

        if ((unsigned int)(list_pane->FocusedIndex()) < filteredpackages.size()) {
            pkg = filteredpackages[list_pane->FocusedIndex()];
            printinfosection("Name: ", pkg->name());
            printinfosection("Version: ", pkg->version());
            printinfosection("Url: ", pkg->url());
            printinfosection("Repo: ", pkg->dbname());
            printinfosection("Packager: ", pkg->packager());
            printinfosection("Builddate: ", pkg->builddate());
            printinfosection("install State: ", pkg->reasonstring());
            printinfosection("Desc: ", pkg->desc());
        }

        refresh();
        list_pane->Refresh();
        if (rightpane == RPE_INFO)
            info_pane->Refresh();
        else
            queue_pane->Refresh();

        if (mode == MODE_INPUT) {
            input_pane->PrintW(op + searchphrase);
            input_pane->Refresh();
        }
    } else if (mode == MODE_HELP) {
        help_pane->Clear();
        print_help();
        help_pane->Refresh();
    }
}

void Program::filterpackages(std::string searchphrase) {

    filteredpackages = packages;

    if (searchphrase.length() == 0)
        return;

    /* if search phrase is alphanumeric only,
       perform a fast and simple search, else run slower regexp search */

    sregex resimple = sregex::compile("[:alnum:]+");
    smatch what;

    if (regex_match(searchphrase, what, resimple)) {
        for (std::vector<Package*>::iterator it = std::find_if(filteredpackages.begin(),
                                                     filteredpackages.end(),
                                                     boost::bind(&Package::matches, _1, searchphrase, op));
            it != filteredpackages.end();
            it = std::find_if(filteredpackages.begin(),
                              filteredpackages.end(),
                              boost::bind(&Package::matches, _1, searchphrase, op)))
            filteredpackages.erase(it);
    } else {
        sregex needle = sregex::compile(searchphrase);
        for (std::vector<Package*>::iterator it = std::find_if(filteredpackages.begin(),
                                                     filteredpackages.end(),
                                                     boost::bind(&Package::matchesre, _1, needle, op));
            it != filteredpackages.end();
            it = std::find_if(filteredpackages.begin(),
                              filteredpackages.end(),
                              boost::bind(&Package::matchesre, _1, needle, op)))
            filteredpackages.erase(it);
    }
}
