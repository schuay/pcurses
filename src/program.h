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

#include "config.h"
#include "history.h"
#include "state.h"

class Package;

class Program
{
public:
    Program();
    ~Program();

    void init();
    void mainloop();

private:
    void run_cmd(const std::string &cmd) const;
    void loadpkgs();
    void init_misc();
    void deinit();
    void clearfilter();
    void filterpackages(const std::string &str);
    void sortpackages(const std::string &str);
    void searchpackages(const std::string &str);
    void execctrl(const ControlOperationEnum op);
    void execmacro(const std::string &str);
    void execmd(const std::string &str);
    void colorcodepackages(const std::string &str);
    void colorcodepackages(const AttributeEnum attr);
    void exitinputmode(FilterOperationEnum o);
    void prepinputmode(FilterOperationEnum o);
    History *gethis(FilterOperationEnum o);

    State state;

    Config conf;

    bool quit;

    std::vector<Package *> packages,
        filteredpackages,
        opqueue;

    std::map<std::string, std::string> macros;

    History hisfilter,
            hissort,
            hissearch,
            hiscolorcode,
            hisexec,
            hismacro;
};

#endif // PROGRAM_H
