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

#include "alpmexception.h"

using std::string;
using std::vector;
using std::cerr;
using std::endl;

class Config
{
public:
    Config();
    ~Config();

    void parse();

    string getConfigFile() const { return configfile; }
    string getRootDir() const { return rootdir; }
    string getDBPath() const { return dbpath; }
    string getLogFile() const { return logfile; }

    vector<string> getRepos() const { return repos; }

private:

    string getconfvalue(const string) const;

    string
            configfile,
            rootdir,
            dbpath,
            logfile;
    vector<string>
            repos;

    enum ConfSection {
        CS_NONE,
        CS_OPTIONS,
        CS_REPO
    };
};

#endif // CONFIG_H
