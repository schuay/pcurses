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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "globals.h"
#include "pcursesexception.h"
#include "program.h"

static char *opt_conf_file = nullptr;

static void usage()
{
    fprintf(stderr,
            "Usage: %s [-h] [-v] [-f CONF_FILE]\n"
            "\n"
            "Arguments:\n"
            "----------\n"
            "-h:            print this message\n"
            "-v:            print version info\n"
            "-f:            specify an alternate config file location\n"
            "\n"
            "Detailed help can be found the README and CONCEPT files located at\n"
            "https://github.com/schuay/pcurses\n"
            "\n"
            "Keyboard shortcuts:\n"
            "-------------------\n"
            "esc:           cancel\n"
            "q:             quit\n"
            "1 to 0:        hotkeys (as configured in %s.conf)\n"
            "!:             execute command, replacing %%p with selected package names\n"
            "@:             run the specified macro (as configured in %s.conf)\n"
            "%%:             run the specified control command (for example, %%filter_clear)\n"
            "r:             reload package info\n"
            "/:             filter packages by specified fields (using regexp)\n"
            "n:             filter packages by name (using regexp)\n"
            "c:             clear all package filters\n"
            "C:             clear the package queue\n"
            "?:             search packages\n"
            ".:             sort packages by specified field\n"
            ";:             colorcode packages by specified field\n"
            "tab:           switch focus between list and queue panes\n"
            "left/right:    add/remove packages from the queue\n"
            "up/down,\n"
            "pg up/down,\n"
            "home/end:      navigation\n"
            "up/down:       browse history while in input mode\n"
            "\n"
            "Command syntax:\n"
            "-------\n"
            "Filter, search, sort, and colorcode operations all use slight variations of the\n"
            "same syntax:\n"
            "\n"
            "<operator>[[<field list>][!]:]<regex>\n"
            "\n"
            "where <operator> is the appropriate operator char (/ for filter),\n"
            "<field list> is an optional list of fields to search (n for package name)\n"
            "! negates the query and filters all non-matching packages\n"
            "and regex specifies the search terms.\n"
            "\n"
            "Examples:\n"
            "\n"
            "/nc:gnome displays all packages with either name or description matching 'gnome'\n"
            "/gnome displays the same results (name & desc are default fields)\n"
            "/nc!:gnome displays the complement to the previous query\n"
            "\n"
            "Control commands:\n"
            "-----------------\n"
            "The following strings may be used as control commands:\n"
            "\n"
            "scroll_up, scroll_down, scroll_home, scroll_end, scroll_pageup,scroll_pagedown,\n"
            "switch_focus,queue_push,queue_pop,queue_clear,help,quit,reload,filter_clear\n",
            APPLICATION_NAME, APPLICATION_NAME, APPLICATION_NAME);
}

static void parseargs(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "hvf:")) != -1) {
        switch (opt) {
        case 'f':
            opt_conf_file = optarg;
            break;
        case 'v':
            fprintf(stdout, "%s %d\n", APPLICATION_NAME, VERSION);
            exit(EXIT_SUCCESS);
            break;
        case 'h':
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    std::string err;

    parseargs(argc, argv);

    Program *p = new Program();

    try {
        p->init(opt_conf_file);
        p->mainloop();
    } catch (PcursesException e) {
        err = e.getmessage();
    } catch (...) {
        err = "guru meditation";
    }

    delete p;

    /* printing any errors at this point, because the Program destructor
      clears the screen */
    if (!err.empty()) {
        std::cerr << err << std::endl;

    }

    return 0;
}
