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
#include <map>

class Config
{
public:
    Config();
    ~Config();

    void parse_pacmanconf();
    void parse_pcursesconf();

    std::string getpacmanconffile() const {
        return pacmanconffile;
    }
    std::string getpcursesconffile() const {
        return pcursesconffile;
    }
    std::string getrootdir() const {
        return rootdir;
    }
    std::string getdbpath() const {
        return dbpath;
    }
    std::string getlogfile() const {
        return logfile;
    }

    std::vector<std::string> getrepos() const {
        return repos;
    }
    std::map<std::string, std::string> getmacros() const {
        return macros;
    }

private:

    std::string getconfvalue(const std::string) const;

    std::string pacmanconffile,
        pcursesconffile,
        rootdir,
        dbpath,
        logfile;

    std::vector<std::string> repos;

    std::map<std::string, std::string> macros;

    enum ConfSection {
        CS_NONE,
        CS_OPTIONS,
        CS_REPO
    };
};

#endif // CONFIG_H
