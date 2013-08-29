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

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <sys/wait.h>

#include "cursesframe.h"
#include "curseslistbox.h"
#include "cursesui.h"
#include "filter.h"
#include "package.h"
#include "pcursesexception.h"

using std::string;
using std::vector;
using std::map;
using boost::shared_ptr;
using boost::xpressive::regex_constants::icase;
using boost::xpressive::smatch;
using boost::xpressive::sregex;

typedef struct __alpm_list_t alpm_list_t;

#define PIPE_READ 0
#define PIPE_WRITE 1

#define KEY_ESC (27)
#define KEY_RETURN (10)
#define KEY_SPACE (32)
#define KEY_TAB (9)
#define KEY_KONSOLEBACKSPACE (127)

Program::Program()
{
    quit = false;
}

Program::~Program()
{
    deinit();
}

void Program::deinit()
{
    CursesUi::ui().disable_curses();

    for (uint i = 0; i < packages.size(); i++) {
        delete packages[i];
    }

    filteredpackages.clear();
    packages.clear();
    opqueue.clear();
}

void Program::run_cmd(const string &cmd) const
{
    pid_t pid;
    int status;


    pid = fork();
    if (pid == 0) {
        /* child */
        execlp("bash", "bash", "-ic", cmd.c_str(), (char *)NULL);
    } else {
        /* parent (or error, which we blissfully ignore */

        /* ignore SIGTTOU (prevent getting sent to the background
          after run_cmd) */
        __sighandler_t handler = signal(SIGTTOU, SIG_IGN);

        waitpid(pid, &status, 0);
        tcsetpgrp(STDIN_FILENO, getpgid(0));    /* regain control of the terminal */
        std::cout << "press return to continue...";
        std::cin.get();

        /* and restore the original handler when done */
        signal(SIGTTOU, handler);
    }
}

void Program::init_misc()
{
    colorcodepackages(state.coloredby);
    state.searchphrases.clear();

    /* exec startup macro if it exists */
    execmacro("startup");
}

void Program::init()
{
    loadpkgs();

    CursesUi::ui().enable_curses(&filteredpackages, &opqueue);

    init_misc();

    CursesUi::ui().update_display(state);
}

void Program::mainloop()
{
    int ch;
    while (!quit) {
        ch = getch();

        /* If a resize has been requested, handle it. */
        CursesUi::ui().handle_resize(state);

        if (ch == ERR || ch == KEY_RESIZE) {
            continue;
        }

        if (state.mode == MODE_STANDARD) {
            switch (ch) {
            case 'k':
            case KEY_UP:
                CursesUi::ui().focused()->move(-1);
                break;
            case 'j':
            case KEY_DOWN:
                CursesUi::ui().focused()->move(1);
                break;
            case KEY_HOME:
                CursesUi::ui().focused()->moveabs(0);
                break;
            case KEY_END:
                CursesUi::ui().focused()->movetoend();
                break;
            case KEY_PPAGE:   /* page up */
                CursesUi::ui().focused()->move(CursesUi::ui().list()->usableheight() * -1);
                break;
            case KEY_NPAGE:   /* page down */
                CursesUi::ui().focused()->move(CursesUi::ui().list()->usableheight());
                break;
            case KEY_TAB:
                CursesUi::ui().switch_focus();
                break;
            case KEY_RIGHT:
                if (CursesUi::ui().focused() != CursesUi::ui().list()) {
                    break;
                }
                if (filteredpackages.size() == 0) {
                    break;
                }

                if (std::find(opqueue.begin(), opqueue.end(),
                              filteredpackages[CursesUi::ui().list()->focusedindex()]) != opqueue.end()) {
                    break;
                }
                opqueue.push_back(filteredpackages[CursesUi::ui().list()->focusedindex()]);
                CursesUi::ui().queue()->movetoend();
                CursesUi::ui().focused()->move(1);
                break;
            case KEY_LEFT:
                if (CursesUi::ui().focused() != CursesUi::ui().queue()) {
                    break;
                }
                CursesUi::ui().queue()->removeselected();
                if (opqueue.empty()) {
                    CursesUi::ui().set_focus(PANE_LIST);
                }
                break;
            case 'C':
                while (!opqueue.empty()) {
                    CursesUi::ui().queue()->removeselected();
                }
                CursesUi::ui().set_focus(PANE_LIST);
                break;
            case 'h':
                state.mode = MODE_HELP;
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
            case 'r':
                deinit();
                init();
            case 'c':
                clearfilter();
                break;
            case 'n':
            case 'd':
                prepinputmode(OP_FILTER);
                state.inputbuf.set(string(1, ch) + ":");
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
        } else if (state.mode == MODE_INPUT) {
            switch (ch) {
            case KEY_ESC:
                exitinputmode(OP_NONE);
                break;
            case KEY_RETURN:
                exitinputmode(state.op);
                break;
            case KEY_DC:
                state.inputbuf.del();
                break;
            case KEY_BACKSPACE:
            case KEY_KONSOLEBACKSPACE:
                state.inputbuf.backspace();
                break;
            case KEY_LEFT:
                state.inputbuf.moveleft();
                break;
            case KEY_RIGHT:
                state.inputbuf.moveright();
                break;
            case KEY_HOME:
                state.inputbuf.movestart();
                break;
            case KEY_END:
                state.inputbuf.moveend();
                break;
            case KEY_UP:
                if (!gethis(state.op)->empty()) {
                    state.inputbuf.set(gethis(state.op)->moveback());
                }
                break;
            case KEY_DOWN:
                if (!gethis(state.op)->empty()) {
                    state.inputbuf.set(gethis(state.op)->moveforward());
                }
                break;
            default:
                state.inputbuf.insert(ch);
                break;
            }
        } else if (state.mode == MODE_HELP) {
            /* exit help screen with any key */
            state.mode = MODE_STANDARD;
        }

        CursesUi::ui().update_display(state);
    }
}

void Program::prepinputmode(FilterOperationEnum o)
{
    state.mode = MODE_INPUT;
    curs_set(1);
    state.inputbuf.clear();
    gethis(o)->reset();
    state.op = o;
}

void Program::exitinputmode(FilterOperationEnum o)
{
    state.mode = MODE_STANDARD;
    curs_set(0);

    state.op = OP_NONE;

    if (state.inputbuf.getcontents().length() == 0) {
        return;
    }

    switch (o) {
    case OP_FILTER:
        filterpackages(state.inputbuf.getcontents());
        flushinp();
        break;
    case OP_SORT:
        sortpackages(state.inputbuf.getcontents());
        break;
    case OP_SEARCH:
        searchpackages(state.inputbuf.getcontents());
        break;
    case OP_COLORCODE:
        colorcodepackages(state.inputbuf.getcontents());
        break;
    case OP_EXEC:
        execmd(state.inputbuf.getcontents());
        break;
    case OP_MACRO:
        execmacro(state.inputbuf.getcontents());
        break;
    default:
        break;
    }
}

void Program::loadpkgs()
{
    std::cout << "Reading package dbs, please wait..." << std::endl;

    _alpm_errno_t err;

    conf.parse_pacmanconf();
    conf.parse_pcursesconf();
    macros = conf.getmacros();


    alpm_handle_t *handle =
        alpm_initialize(conf.getrootdir().c_str(), conf.getdbpath().c_str(), &err);
    if (handle == NULL) {
        throw PcursesException(alpm_strerror(err));
    }

    alpm_option_set_logfile(handle, conf.getlogfile().c_str());

    vector<string> repos = conf.getrepos();
    for (uint i = 0; i < repos.size(); i++) {
        /* i'm going to be lazy here and remind myself to handle siglevel properly later on */
        alpm_register_syncdb(handle, repos[i].c_str(), ALPM_SIG_USE_DEFAULT);
    }

    alpm_db_t *localdb = alpm_get_localdb(handle);


    /* create our package list */
    alpm_list_t *dbs = alpm_list_copy(alpm_get_syncdbs(handle));
    dbs = alpm_list_add(dbs, localdb);
    for (; dbs; dbs = alpm_list_next(dbs)) {
        alpm_db_t *db = (alpm_db_t *)dbs->data;
        for (alpm_list_t *pkgs = alpm_db_get_pkgcache(db); pkgs; pkgs = alpm_list_next(pkgs)) {
            alpm_pkg_t *pkg = (alpm_pkg_t *)pkgs->data;
            Package *p = new Package(pkg, localdb);
            Package *parray[] = { p };
            if (!std::includes(packages.begin(), packages.end(),
                               parray, parray + 1,
                               boost::bind(&Filter::cmp, _1, _2, A_NAME))) {
                packages.push_back(p);
            } else {
                delete p;
            }
        }
        std::sort(packages.begin(), packages.end(), boost::bind(&Filter::cmp, _1, _2, A_NAME));
    }
    alpm_list_free(dbs);

    if (alpm_release(handle) != 0) {
        throw PcursesException("failed to deinitialize alpm library");
    }


    filteredpackages = packages;
}

void Program::clearfilter()
{
    filteredpackages = packages;
    std::sort(filteredpackages.begin(), filteredpackages.end(),
              boost::bind(&Filter::cmp, _1, _2, state.sortedby));

    state.searchphrases = "";
    CursesUi::ui().list()->moveabs(0);
}

History *Program::gethis(FilterOperationEnum o)
{
    History *v = NULL;

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

void Program::execmacro(const string &str)
{
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

        state.inputbuf.set(cmd.substr(1));
        exitinputmode(op);
    }
}

void Program::execmd(const string &str)
{
    gethis(OP_EXEC)->add(str);

    string pkgs = "";
    for (uint i = 0; i < opqueue.size(); i++) {
        pkgs += opqueue[i]->getname() + " ";
    }

    const string needle = "%p";
    size_t  pos;
    string processed_str = str;
    while ((pos = processed_str.find(needle)) != string::npos) {
        processed_str.replace(pos, needle.length(), pkgs);
    }

    CursesUi::ui().disable_curses();
    run_cmd(processed_str);
    CursesUi::ui().enable_curses(&filteredpackages, &opqueue);
}

void Program::colorcodepackages(const string &str)
{
    if (str.length() < 1) {
        return;
    }

    gethis(OP_COLORCODE)->add(str);

    AttributeEnum attr = A_NONE;
    uint i = 0;

    while (attr == A_NONE && i < str.length()) {
        attr = AttributeInfo::chartoattr(str[i]);
        i++;
    }

    if (attr != A_NONE) {
        colorcodepackages(attr);
    }
}

void Program::colorcodepackages(const AttributeEnum attr)
{
    Filter::clearattrs();

    for (auto p : packages) {
        Filter::assigncol(p, attr);
    }

    state.coloredby = attr;
}

void Program::searchpackages(const string &str)
{
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
        return;
    }

    /* we start the search at the current package */
    vector<Package *>::iterator begin = filteredpackages.begin() + CursesUi::ui().list()->focusedindex()
                                        + 1;
    vector<Package *>::iterator it;

    it = std::find_if(begin, filteredpackages.end(),
                      boost::bind(&Filter::matches, _1, searchphrase));

    /* if not found (and original search didn't start at beginning) wrap around */
    if (it == filteredpackages.end() && begin != filteredpackages.begin()) {
        it = std::find_if(filteredpackages.begin(), filteredpackages.end(),
                          boost::bind(&Filter::matches, _1, searchphrase));
    }

    /* not found, do nothing */
    if (it == filteredpackages.end()) {
        return;
    }

    /* move focus to found pkg */
    CursesUi::ui().list()->moveabs(it - filteredpackages.begin());
}

void Program::sortpackages(const string &str)
{
    if (str.length() < 1) {
        return;
    }

    gethis(OP_SORT)->add(str);

    AttributeEnum attr = A_NONE;
    uint i = 0;

    while (attr == A_NONE && i < str.length()) {
        attr = AttributeInfo::chartoattr(str[i]);
        i++;
    }

    if (attr == A_NONE) {
        return;
    }

    state.sortedby = attr;

    std::sort(filteredpackages.begin(), filteredpackages.end(),
              boost::bind(&Filter::cmp, _1, _2, attr));
}

void Program::filterpackages(const string &str)
{
    string fieldlist, searchphrase, negate;

    gethis(OP_FILTER)->add(str);

    /* first, split actual search phrase from field prefix */
    sregex reprefix = sregex::compile("^(([A-Za-zq]*)([!]?):)?(.*)");
    smatch what;

    Filter::clearattrs();
    if (regex_search(str, what, reprefix)) {
        fieldlist = what[2];
        negate = what[3];
        searchphrase = what[4];
    } else {
        throw PcursesException("Could not match filter regex.");
    }

    /* if search phrase is empty, nothing to do */
    if (searchphrase.length() == 0) {
        return;
    }

    if (!fieldlist.empty()) {
        Filter::setattrs(fieldlist);
    }

    /* if search phrase is alphanumeric only,
       perform a fast and simple search, else run slower regexp search */

    sregex resimple = sregex::compile("[:alnum:]+");

    /* catch invalid regex input by user */
    try {
        if (regex_match(searchphrase, what, resimple)) {
            bool (*fn)(const Package *, const string) = negate.empty()
                    ? &Filter::notmatches : &Filter::matches;
            vector<Package *>::iterator it =
                std::find_if(filteredpackages.begin(), filteredpackages.end(),
                             boost::bind(fn, _1, searchphrase));
            while (it != filteredpackages.end()) {
                filteredpackages.erase(it);
                it = std::find_if(it, filteredpackages.end(),
                                  boost::bind(fn, _1, searchphrase));
            }
        } else {
            bool (*fn)(const Package *, const sregex) = negate.empty()
                    ? &Filter::notmatchesre : &Filter::matchesre;
            sregex needle = sregex::compile(searchphrase, icase);
            vector<Package *>::iterator it =
                std::find_if(filteredpackages.begin(), filteredpackages.end(),
                             boost::bind(fn, _1, needle));
            while (it != filteredpackages.end()) {
                filteredpackages.erase(it);
                it = std::find_if(it, filteredpackages.end(),
                                  boost::bind(fn, _1, needle));
            }
        }

        if (state.searchphrases.length() != 0) {
            state.searchphrases += ", ";
        }
        state.searchphrases += str;

        /* List contents have changed, move to beginning. */
        CursesUi::ui().list()->moveabs(0);
    } catch (boost::xpressive::regex_error &e) {
        /* we don't have any decent feedback mechanisms, so ignore faulty regexp */
    }
}
