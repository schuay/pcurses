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

#ifndef PACKAGE_H
#define PACKAGE_H

#include <alpm.h>
#include <string>
#include <ctime>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>

#include "pcursesexception.h"
#include "attributeinfo.h"

typedef struct __alpm_pkg_t alpm_pkg_t;

using std::string;
using std::vector;
using boost::xpressive::sregex;
using boost::xpressive::smatch;
using boost::xpressive::regex_constants::icase;

enum InstallReasonEnum {
    IRE_EXPLICIT,
    IRE_ASDEPS,
    IRE_NOTINSTALLED
};

enum OperationEnum {
    OE_INSTALL_EXPLICIT,
    OE_INSTALL_ASDEPS,
    OE_UPGRADE,
    OE_REMOVE
};


class Package
{
public:
    Package(alpm_pkg_t *pkg, alpm_db_t *localdb);

    string getarch() const;
    string getbuilddate() const;
    string getconflicts() const;
    string getdepends() const;
    string getdesc() const;
    string getgroups() const;
    string getisize() const;
    string getlicenses() const;
    string getname() const;
    string getoptdepends() const;
    string getpackager() const;
    string getprovides() const;
    string getreason() const;
    string getreplaces() const;
    string getrepo() const;
    string getsize() const;
    string getupdatestate() const;
    string geturl() const;
    string getversion() const;

    string getattr(AttributeEnum attr) const;
    unsigned long getulongattr(AttributeEnum attr) const;

    void setcolindex(int index);
    int getcolindex() const;

    void setop(OperationEnum oe);
    OperationEnum getop() const;

private:

    string trimstr(const char *c) const;
    string list2str(alpm_list_t *l, string delim) const;
    static string size2str(unsigned long size);

    string
            _name,
            _url,
            _packager,
            _desc,
            _version,
            _dbname,
            _arch,
            _licenses,
            _groups,
            _depends,
            _optdepends,
            _conflicts,
            _provides,
            _replaces,
            _sizestr,
            _installsizestr,
            _updatestate,
            _localversion;

    int
            _colindex;

    unsigned long
            _size,
            _installsize;

    time_t
            _builddate;

    InstallReasonEnum
            _reason;

    OperationEnum
            op;
};

#endif // PACKAGE_H
