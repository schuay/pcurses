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
    op = OP_NONE;
    rightpane = RPE_INFO;
    mode = MODE_STANDARD;
    sortedby = A_NAME;
    coloredby = A_INSTALLSTATE;
}
Program::~Program() {
    deinit();
}

void Program::deinit() {
    deinit_curses();
    system("clear");

    for (unsigned int i = 0; i < packages.size(); i++)
        delete packages[i];

    filteredpackages.clear();
    packages.clear();
    opqueue.clear();

    if (alpm_release(handle) != 0)
        throw PcursesException("failed to deinitialize alpm library");
}

void Program::run_cmd(string cmd) const {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        /* child */
        execlp("bash", "bash", "-ic", cmd.c_str(), (char*)NULL);
    } else {
        /* parent (or error, which we blissfully ignore */
        waitpid(pid, &status, 0);
        tcsetpgrp(STDIN_FILENO, getpgid(0));    /* regain control of the terminal */
        std::cout << "press return to continue...";
        std::cin.get();
    }
}

void Program::init() {

    std::cout << "Reading package dbs, please wait..." << std::endl;

    /* initialize libalpm */
    init_alpm();

    /* create our package list */
    alpm_list_t *dbs = alpm_list_copy(alpm_option_get_syncdbs(handle));
    dbs = alpm_list_add(dbs, localdb);
    for ( ; dbs; dbs = alpm_list_next(dbs)) {
        alpm_db_t *db = (alpm_db_t*)alpm_list_getdata(dbs);
        for (alpm_list_t *pkgs = alpm_db_get_pkgcache(db); pkgs; pkgs = alpm_list_next(pkgs)) {
            alpm_pkg_t *pkg = (alpm_pkg_t*)alpm_list_getdata(pkgs);
            Package *p = new Package(pkg, localdb);
            Package *parray[] = { p };
            if (!std::includes(packages.begin(), packages.end(),
                              parray, parray + 1,
                              boost::bind(&Filter::cmp, _1, _2, A_NAME))) {
                packages.push_back(p);
            }
            else
                delete p;
        }
        std::sort(packages.begin(), packages.end(), boost::bind(&Filter::cmp, _1, _2, A_NAME));
    }
    alpm_list_free(dbs);

    filteredpackages = packages;

    /* initial color coding */
    colorcodepackages(string(1,AttributeInfo::attrtochar(coloredby)));

    /* initialize curses */
    system("clear");
    init_curses();
    setfocus(list_pane);
    rightpane = RPE_INFO;
    list_pane->setlist(&filteredpackages);
    queue_pane->setlist(&opqueue);

    /* exec startup macro if it exists */
    execmacro("startup");

    updatelistinfo();
    updatedisplay();
}

void Program::setfocus(CursesListBox *frame) {
    list_pane->setfocused(false);
    queue_pane->setfocused(false);

    focused_pane = frame;
    frame->setfocused(true);
}

void Program::mainloop() {
    int ch;
    while (!quit) {
        ch = getch();

        if (mode == MODE_STANDARD) {
            switch (ch) {
            case 'k':
            case KEY_UP:
                focused_pane->move(-1);
                break;
            case 'j':
            case KEY_DOWN:
                focused_pane->move(1);
                break;
            case KEY_HOME:
                focused_pane->moveabs(0);
                break;
            case KEY_END:
                focused_pane->movetoend();
                break;
            case KEY_PPAGE:   /* page up */
                focused_pane->move(list_pane->usableheight() * -1);
                break;
            case KEY_NPAGE:   /* page down */
                focused_pane->move(list_pane->usableheight());
                break;
            case KEY_TAB:
                if (rightpane == RPE_QUEUE) {
                    setfocus((focused_pane == list_pane) ? queue_pane : list_pane);
                }
                break;
            case KEY_SPACE:
                if (rightpane == RPE_QUEUE) {
                    rightpane = RPE_INFO;
                    setfocus(list_pane);
                } else {
                    rightpane = RPE_QUEUE;
                }
                break;
            case KEY_RIGHT:
                if (focused_pane != list_pane) break;
                if (filteredpackages.size() == 0) break;

                if (rightpane != RPE_QUEUE) rightpane = RPE_QUEUE;
                if (std::find(opqueue.begin(), opqueue.end(), filteredpackages[list_pane->focusedindex()]) != opqueue.end()) {
                    break;
                }
                opqueue.push_back(filteredpackages[list_pane->focusedindex()]);
                queue_pane->movetoend();
                focused_pane->move(1);
                break;
            case KEY_LEFT:
                if (focused_pane != queue_pane) break;
                queue_pane->removeselected();
                break;
            case 'h':
                mode = MODE_HELP;
                break;
            case 'q':
                quit = true;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                execmacro(string(1, ch));
                break;
            case 'c':
                clearfilter();
                break;
            case 'n':
            case 'd':
                prepinputmode(OP_FILTER);
                inputbuf.set(string(1, ch) + ":");
                break;
            case '/':
            case '.':
            case '?':
            case ';':
            case '!':
            case '@':
                prepinputmode(strtoopt(string(1, ch)));
                break;
            default:
                break;
            }
        } else if (mode == MODE_INPUT) {
            switch (ch) {
            case KEY_ESC:
                exitinputmode(OP_NONE);
                break;
            case KEY_RETURN:
                exitinputmode(op);
                break;
            case KEY_DC:
                inputbuf.del();
                break;
            case KEY_BACKSPACE:
            case KEY_KONSOLEBACKSPACE:
                inputbuf.backspace();
                break;
            case KEY_LEFT:
                inputbuf.moveleft();
                break;
            case KEY_RIGHT:
                inputbuf.moveright();
                break;
            case KEY_UP:
                inputbuf.set(gethis(op)->moveback());
                break;
            case KEY_DOWN:
                inputbuf.set(gethis(op)->moveforward());
                break;
            default:
                inputbuf.insert(ch);
                break;
            }
        } else if (mode == MODE_HELP) {
            /* exit help screen with any key */
            mode = MODE_STANDARD;
        }

        updatedisplay();
    }
}

void Program::prepinputmode(FilterOperationEnum o) {
    mode = MODE_INPUT;
    curs_set(1);
    inputbuf.clear();
    gethis(o)->reset();
    op = o;
}
void Program::exitinputmode(FilterOperationEnum o) {
    mode = MODE_STANDARD;
    curs_set(0);

    op = OP_NONE;

    if (inputbuf.getcontents().length() == 0) {
        return;
    }

    switch (o) {
    case OP_FILTER:
        displayprocessingmsg();
        filterpackages(inputbuf.getcontents());
        list_pane->setlist(&filteredpackages);
        flushinp();
        break;
    case OP_SORT:
        sortpackages(inputbuf.getcontents());
        break;
    case OP_SEARCH:
        searchpackages(inputbuf.getcontents());
        break;
    case OP_COLORCODE:
        colorcodepackages(inputbuf.getcontents());
        break;
    case OP_EXEC:
        execmd(inputbuf.getcontents());
        break;
    case OP_MACRO:
        execmacro(inputbuf.getcontents());
        break;
    default:
        break;
    }
}

void Program::displayprocessingmsg() {
    list_pane->setfooter("Processing...");
    updatedisplay();
}

#define PRINTH(a, b) help_pane->printw(a, A_BOLD); help_pane->printw(b);
void Program::print_help() {
    help_pane->printw("COMMANDS\n", A_BOLD);
    help_pane->printw("\n");
    PRINTH("esc: ", "cancel\n");
    PRINTH("q: ", "quit\n");
    PRINTH("!: ", "execute the given command, replacing %p with the package names\n");
    PRINTH("@: ", "run the specified macro (as configured in " APPLICATION_NAME ".conf\n");
    PRINTH("/: ", "filter packages by specified fields (using regexp)\n");
    PRINTH("", "   note that filters can be chained.\n")
    PRINTH("n: ", "filter packages by name (using regexp)\n");
    PRINTH("c: ", "clear all package filters\n");
    PRINTH("?: ", "search packages\n");
    PRINTH(".: ", "sort packages by specified field\n");
    PRINTH(";: ", "colorcode packages by specified field\n");
    PRINTH("tab: ", "switch focus between list and queue panes\n");
    PRINTH("spacebar: ", "switch between displaying the queue and info panes\n");
    PRINTH("left/right arrows: ", "add/remove packages from the queue\n");
    PRINTH("up/down arrows, pg up/down, home/end: ", "navigation\n");
    PRINTH("up/down arrows (in input mode): ", "browse history\n");
}
#undef PRINTH

void Program::init_alpm() {
    _alpm_errno_t err;

    Config conf;
    conf.parse_pacmanconf();
    conf.parse_pcursesconf();
    macros = conf.getmacros();

    handle = alpm_initialize(conf.getrootdir().c_str(), conf.getdbpath().c_str(), &err);
    if (handle == NULL) {
        throw PcursesException(alpm_strerror(err));
    }

    alpm_option_set_logfile(handle, conf.getlogfile().c_str());

    vector<string> repos = conf.getrepos();
    for (unsigned int i = 0; i < repos.size(); i++) {
        /* i'm going to be lazy here and remind myself to handle siglevel properly later on */
        alpm_db_register_sync(handle, repos[i].c_str(), ALPM_SIG_USE_DEFAULT);
    }

    localdb = alpm_option_get_localdb(handle);
}

void Program::init_curses() {

    setlocale(LC_ALL, "");

    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();

    /* target ist archlinux so we know a proper ncurses will be used.
       otherwise we would need to conditionally include this using
       NCURSES_VERSION */
    use_default_colors();

    init_pair(5, -1, -1);                   /* default (pane BG) */
    init_pair(2, COLOR_GREEN, -1);          /* default highlight 1 */
    init_pair(3, COLOR_CYAN, -1);           /* default highlight 2 */
    init_pair(6, COLOR_BLUE, -1);
    init_pair(7, COLOR_MAGENTA, -1);
    init_pair(8, COLOR_RED, -1);
    init_pair(9, COLOR_YELLOW, -1);
    init_pair(1, COLOR_BLACK, COLOR_WHITE); /* inverted (status bar BG) */
    init_pair(4, COLOR_BLUE, COLOR_WHITE);  /* inverted highlight 1 */

    const int lwidth = 30;
    const int height = LINES - 1;
    list_pane = new CursesListBox(lwidth, height, 0, 0, true);
    info_pane = new CursesFrame(COLS - lwidth + 1, height, 0, lwidth - 1, true);
    queue_pane = new CursesListBox(COLS - lwidth + 1, height, 0, lwidth - 1, true);
    status_pane = new CursesFrame(COLS, 1, LINES - 1, 0, false);
    input_pane = new CursesFrame(COLS, 3, LINES - 2, 0, true);
    help_pane = new CursesFrame(COLS - 10, 20, 1, 5, true);

    list_pane->setheader("Packages");
    info_pane->setheader("Info");
    info_pane->setfooter("Press h for help");
    queue_pane->setheader("Queue");
    input_pane->setheader("Input");
    help_pane->setheader("Help");

    list_pane->setbackground(C_DEF);
    info_pane->setbackground(C_DEF);
    queue_pane->setbackground(C_DEF);
    status_pane->setbackground(C_INV);
    input_pane->setbackground(C_DEF);
    help_pane->setbackground(C_DEF);
}
void Program::deinit_curses() {
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
}

void Program::printinfosection(AttributeEnum attr, string text) {

    string caption = AttributeInfo::attrname(attr);
    char hllower = AttributeInfo::attrtochar(attr);
    char hlupper = toupper(hllower);
    bool hldone = false;
    int style;

    for (unsigned int i = 0; i < caption.size(); i++) {
        if (!hldone && (caption[i] == hllower || caption[i] == hlupper)) {
            style = C_DEF;
            hldone = true;
        }
        else style = C_DEF_HL2;


        info_pane->printw(string(1, caption[i]), style);
    }
    info_pane->printw(": ", C_DEF_HL2);

    string txt = text + "\n";
    info_pane->printw(txt);
}
void Program::updatedisplay() {

    /* this runs **at least** once per loop iteration
       for example it can run more than once if we need to display
       a 'processing' message during filtering
     */

    if (mode == MODE_INPUT || mode == MODE_STANDARD) {
        Package *pkg;

        erase();
        list_pane->clear();
        info_pane->clear();
        status_pane->clear();
        input_pane->clear();
        queue_pane->clear();

        /* info pane */
        if ((unsigned int)(list_pane->focusedindex()) < filteredpackages.size()) {
            pkg = filteredpackages[list_pane->focusedindex()];
            for (int i = 0; i < A_NONE; i++) {
                AttributeEnum attr = (AttributeEnum)i;
                string txt = pkg->getattr(attr);
                if (txt.length() != 0)
                    printinfosection(attr, txt);
            }
        }

        /* status bar */
        status_pane->mvprintw(1, 0, "Sorted by: ", C_INV_HL1);
        status_pane->printw(AttributeInfo::attrname(sortedby), C_INV);
        status_pane->printw(" Colored by: ", C_INV_HL1);
        status_pane->printw(AttributeInfo::attrname(coloredby), C_INV);
        status_pane->printw(" Filtered by: ", C_INV_HL1);
        status_pane->printw(((searchphrases.length() == 0) ? "-" : searchphrases), C_INV);

        wnoutrefresh(stdscr);
        status_pane->refresh();
        list_pane->refresh();
        if (rightpane == RPE_INFO)
            info_pane->refresh();
        else
            queue_pane->refresh();

        if (mode == MODE_INPUT) {
            input_pane->printw(optostr(op) + inputbuf.getcontents());
            input_pane->move(inputbuf.getpos() + 1, 0);
            input_pane->refresh();
        }
    } else if (mode == MODE_HELP) {
        help_pane->clear();
        print_help();
        help_pane->refresh();
    }

    CursesFrame::DoUpdate();
}

string Program::optostr(FilterOperationEnum o) const {
    switch (o) {
    case OP_FILTER: return "/";
    case OP_SORT: return ".";
    case OP_SEARCH: return "?";
    case OP_COLORCODE: return ";";
    case OP_EXEC: return "!";
    case OP_MACRO: return "@";
    case OP_NONE: return "";
    default: assert(0);
    }
}
FilterOperationEnum Program::strtoopt(string str) const {
    for (int i = 0; i < OP_NONE; i++) {
        if (optostr((FilterOperationEnum)i) == str) {
            return (FilterOperationEnum)i;
        }
    }
    return OP_NONE;
}

void Program::clearfilter() {
    filteredpackages = packages;
    std::sort(filteredpackages.begin(), filteredpackages.end(),
              boost::bind(&Filter::cmp, _1, _2, sortedby));

    searchphrases = "";
    updatelistinfo();
}

History *Program::gethis(FilterOperationEnum o) {

    History *v;
    switch (o) {
    case OP_FILTER:
        v = &hisfilter;
        break;
    case OP_SORT:
        v = &hissort;
        break;
    case OP_SEARCH:
        v = &hissearch;
        break;
    case OP_COLORCODE:
        v = &hiscolorcode;
        break;
    case OP_EXEC:
        v = &hisexec;
        break;
    case OP_MACRO:
        v = &hismacro;
        break;
    default:
        assert(0);
    }

    return v;
}

void Program::execmacro(string str) {

    gethis(OP_MACRO)->add(str);

    /* macro delimiter is ',' */
    vector<string> strs;
    boost::split(strs, str, boost::is_any_of(","));

    for (uint i = 0; i < strs.size(); i++) {
        string macropart = strs[i];
        boost::trim(macropart);

        map<string, string>::iterator it;
        it = macros->find(macropart);

        if (it == macros->end()) {
            return;
        }

        string cmd = it->second;

        FilterOperationEnum op = strtoopt(cmd.substr(0, 1));
        if (op == OP_NONE) {
            return;
        }

        inputbuf.set(cmd.substr(1));
        exitinputmode(op);
    }
}

void Program::execmd(string str) {

    gethis(OP_EXEC)->add(str);

    string pkgs = "";
    for (unsigned int i = 0; i < opqueue.size(); i++) {
        pkgs += opqueue[i]->getname() + " ";
    }

    const string needle = "%p";
    size_t  pos;
    while ((pos = str.find(needle)) != string::npos) {
        str.replace(pos, needle.length(), pkgs);
    }

    deinit();
    run_cmd(str);
    init();
}

void Program::colorcodepackages(string str) {
    if (str.length() < 1)
        return;

    gethis(OP_COLORCODE)->add(str);

    AttributeEnum attr = A_NONE;
    unsigned int i = 0;

    while (attr == A_NONE && i < str.length()) {
        attr = AttributeInfo::chartoattr(str[i]);
        i++;
    }

    if (attr == A_NONE)
        return;

    vector<Package*>::iterator it = filteredpackages.begin();
    for (; it != filteredpackages.end(); it++)
        Filter::assigncol(*it, attr);

    coloredby = attr;
}

void Program::searchpackages(string str) {

    string fieldlist, searchphrase;

    gethis(OP_SEARCH)->add(str);

    /* first, split actual search phrase from field prefix */
    sregex reprefix = sregex::compile("^([A-Za-z]*):(.*)");
    smatch what;

    Filter::clearattrs();
    if (regex_search(str, what, reprefix)) {
        fieldlist = what[1];
        searchphrase = what[2];
        Filter::setattrs(fieldlist);
    } else {
        searchphrase = str;
    }

    /* if search phrase is empty, nothing to do */
    if (searchphrase.length() == 0) {
        updatelistinfo();
        return;
    }

    /* we start the search at the current package */
    vector<Package*>::iterator begin = filteredpackages.begin() + list_pane->focusedindex() + 1;
    vector<Package*>::iterator it;

    it = std::find_if(begin, filteredpackages.end(),
                      boost::bind(&Filter::matches, _1, searchphrase));

    /* if not found (and original search didn't start at beginning) wrap around */
    if (it == filteredpackages.end() && begin != filteredpackages.begin()) {
        it = std::find_if(filteredpackages.begin(), filteredpackages.end(),
                          boost::bind(&Filter::matches, _1, searchphrase));
    }

    /* not found, do nothing */
    if (it == filteredpackages.end())
        return;

    /* move focus to found pkg */
    list_pane->moveabs(it - filteredpackages.begin());
}

void Program::sortpackages(string str) {
    if (str.length() < 1)
        return;

    gethis(OP_SORT)->add(str);

    AttributeEnum attr = A_NONE;
    unsigned int i = 0;

    while (attr == A_NONE && i < str.length()) {
        attr = AttributeInfo::chartoattr(str[i]);
        i++;
    }

    if (attr == A_NONE)
        return;

    sortedby = attr;

    std::sort(filteredpackages.begin(), filteredpackages.end(),
              boost::bind(&Filter::cmp, _1, _2, attr));
}

void Program::filterpackages(string str) {

    string fieldlist, searchphrase;

    gethis(OP_FILTER)->add(str);

    /* first, split actual search phrase from field prefix */
    sregex reprefix = sregex::compile("^([A-Za-z]*):(.*)");
    smatch what;

    Filter::clearattrs();
    if (regex_search(str, what, reprefix)) {
        fieldlist = what[1];
        searchphrase = what[2];
        Filter::setattrs(fieldlist);
    } else {
        searchphrase = str;
    }

    /* if search phrase is empty, nothing to do */
    if (searchphrase.length() == 0) {
        updatelistinfo();
        return;
    }

    if (searchphrases.length() != 0) searchphrases += ", ";
    searchphrases += str;

    /* if search phrase is alphanumeric only,
       perform a fast and simple search, else run slower regexp search */

    sregex resimple = sregex::compile("[:alnum:]+");

    if (regex_match(searchphrase, what, resimple)) {
        vector<Package*>::iterator it =
                std::find_if(filteredpackages.begin(), filteredpackages.end(),
                             boost::bind(&Filter::notmatches, _1, searchphrase));
        while (it != filteredpackages.end()) {
            filteredpackages.erase(it);
            it = std::find_if(it, filteredpackages.end(),
                              boost::bind(&Filter::notmatches, _1, searchphrase));
        }
    } else {
        sregex needle = sregex::compile(searchphrase, icase);
        vector<Package*>::iterator it =
                std::find_if(filteredpackages.begin(), filteredpackages.end(),
                             boost::bind(&Filter::notmatchesre, _1, needle));
        while (it != filteredpackages.end()) {
            filteredpackages.erase(it);
            it = std::find_if(it, filteredpackages.end(),
                              boost::bind(&Filter::notmatchesre, _1, needle));
        }
    }

    updatelistinfo();
}
void Program::updatelistinfo() {
    list_pane->setfooter(boost::str(boost::format("%d Package(s)") % filteredpackages.size()));
}
