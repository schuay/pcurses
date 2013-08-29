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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/shared_ptr.hpp>
#include <map>

#include "pcursesexception.h"
#include "globals.h"

using std::string;
using std::vector;
using std::map;
using std::cerr;
using std::endl;
using boost::xpressive::sregex;
using boost::xpressive::smatch;
using boost::xpressive::regex_constants::icase;
using boost::shared_ptr;

class Config
{
public:
    Config();
    ~Config();

    void parse_pacmanconf();
    void parse_pcursesconf();

    string getpacmanconffile() const {
        return pacmanconffile;
    }
    string getpcursesconffile() const {
        return pcursesconffile;
    }
    string getrootdir() const {
        return rootdir;
    }
    string getdbpath() const {
        return dbpath;
    }
    string getlogfile() const {
        return logfile;
    }

    vector<string> getrepos() const {
        return repos;
    }
    shared_ptr<map<string, string> > getmacros() const {
        return macros;
    }

private:

    string getconfvalue(const string) const;

    string pacmanconffile,
           pcursesconffile,
           rootdir,
           dbpath,
           logfile;

    vector<string> repos;

    shared_ptr<map<string, string> > macros;

    enum ConfSection {
        CS_NONE,
        CS_OPTIONS,
        CS_REPO
    };
};

#endif // CONFIG_H
