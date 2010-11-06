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

#include "config.h"

Config::Config()
{
    configfile = "/etc/pacman.conf";
    rootdir = "/";
    dbpath = "/var/lib/pacman";
    logfile = "/var/log/pacman.log";
}
Config::~Config() {
}

string Config::getconfvalue(const string str) const {
    if (str.length() == 0)
        return "";

    size_t found = str.find("=");
    if (found == string::npos)
        return NULL;

    found++;
    for (; found < str.length(); found++)
        if (str[found] != ' ' && str[found != '\t'])
            break;

    return str.substr(found);
}
void Config::parse() {
    const string
            s_rootdir = "RootDir",
            s_dbpath = "DBPath",
            s_logfile = "LogFile";
    std::ifstream
            conf;

    conf.open(configfile.c_str());
    if (!conf.is_open()) {
        throw AlpmException("config file could not be read.");
    }

    ConfSection section = CS_NONE;
    while (conf.good()) {
        string line;
        std::getline(conf, line);

        if (line.length() == 0)
            continue;

        if (line == "[options]") {
            section = CS_OPTIONS;
            continue;
        } else if (line[0] == '[' && line[line.length() - 1]) {
            section = CS_REPO;
            repos.push_back(line.substr(1, line.length() - 2));
            continue;
        }

        if (section != CS_OPTIONS)
            continue;

        if (boost::starts_with(line, s_rootdir)) {
            rootdir = getconfvalue(line);
        } else if (boost::starts_with(line, s_dbpath)) {
            dbpath = getconfvalue(line);
        } else if (boost::starts_with(line, s_logfile)) {
            logfile = getconfvalue(line);
        }
    }

    conf.close();
}
