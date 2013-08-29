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

#include "cursesui.h"
#include "config.h"
#include "filter.h"
#include "history.h"
#include "state.h"

using std::string;
using std::vector;
using std::map;
using boost::shared_ptr;

typedef struct __alpm_list_t alpm_list_t;

#define PIPE_READ 0
#define PIPE_WRITE 1

#define KEY_ESC (27)
#define KEY_RETURN (10)
#define KEY_SPACE (32)
#define KEY_TAB (9)
#define KEY_KONSOLEBACKSPACE (127)

class Program
{
public:
    Program();
    ~Program();

    void init();
    void mainloop();

private:

    void run_cmd(string cmd) const;
    void loadpkgs();
    void init_misc();
    void deinit();
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


    State state;

    Config conf;

    bool quit;

    vector<Package *> packages,
           filteredpackages,
           opqueue;

    shared_ptr<map<string, string> > macros;

    History hisfilter,
            hissort,
            hissearch,
            hiscolorcode,
            hisexec,
            hismacro;
};

#endif // PROGRAM_H