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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <iostream>
#include <ncurses.h>
#include <vector>
#include <map>
#include <stdarg.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>

#include "package.h"
#include "pcursesexception.h"
#include "cursesframe.h"
#include "curseslistbox.h"
#include "config.h"
#include "filter.h"
#include "history.h"
#include "inputbuffer.h"
#include "globals.h"

using std::string;
using std::vector;
using std::map;
using boost::shared_ptr;

typedef struct __pmdb_t pmdb_t;
typedef struct __alpm_list_t alpm_list_t;

#define PIPE_READ 0
#define PIPE_WRITE 1

#define KEY_ESC (27)
#define KEY_RETURN (10)
#define KEY_SPACE (32)
#define KEY_TAB (9)
#define KEY_KONSOLEBACKSPACE (127)

enum ModeEnum {
    MODE_STANDARD,
    MODE_INPUT,
    MODE_HELP
};

enum FilterOperationEnum {
    OP_SEARCH,
    OP_FILTER,
    OP_SORT,
    OP_COLORCODE,
    OP_EXEC,
    OP_MACRO,
    OP_NONE
};

class Program
{
public:
    Program();
    ~Program();

    void init();
    void mainloop();

private:

    void ensureminwsize(uint w, uint h) const;
    void do_resize();
    void setfocus(CursesListBox *frame);
    void run_cmd(string cmd) const;
    void loadpkgs();
    void init_curses();
    void print_help();
    void deinit();
    void deinit_curses();
    void printinfosection(AttributeEnum attr, string text);
    void updatedisplay();
    void clearfilter();
    void filterpackages(string str);
    void sortpackages(string str);
    void searchpackages(string str);
    void execmacro(string str);
    void execmd(string str);
    void colorcodepackages(string str);
    void exitinputmode(FilterOperationEnum o);
    void prepinputmode(FilterOperationEnum o);
    History *gethis(FilterOperationEnum o);
    void displayprocessingmsg();
    string optostr(FilterOperationEnum o) const;
    FilterOperationEnum strtoopt(string str) const;


    ModeEnum mode;

    Config
            conf;

    bool
            quit;

    CursesListBox
            *list_pane,
            *queue_pane,
            *focused_pane;
    CursesFrame
            *info_pane,
            *input_pane,
            *help_pane,
            *status_pane;

    vector<Package*>
            packages,
            filteredpackages,
            opqueue;

    shared_ptr<map<string, string> >
            macros;

    string
            searchphrases;

    InputBuffer
            inputbuf;

    History
            hisfilter,
            hissort,
            hissearch,
            hiscolorcode,
            hisexec,
            hismacro;

    AttributeEnum
            sortedby,
            coloredby;

    FilterOperationEnum
            op;
};

#endif // PROGRAM_H
