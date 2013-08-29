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
#include <vector>

#include "attributeinfo.h"

typedef struct __alpm_pkg_t alpm_pkg_t;

enum InstallReasonEnum {
    IRE_EXPLICIT,
    IRE_ASDEPS,
    IRE_NOTINSTALLED
};

enum UpdateStateEnum {
    USE_NOTINSTALLED,
    USE_UPTODATE,
    USE_UPDATEAVAILABLE
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

    std::string getarch() const;
    std::string getbuilddate() const;
    std::string getconflicts() const;
    std::string getdepends() const;
    std::string getdesc() const;
    std::string getgroups() const;
    std::string getisize() const;
    std::string getlicenses() const;
    std::string getname() const;
    std::string getoptdepends() const;
    std::string getpackager() const;
    std::string getprovides() const;
    std::string getreason() const;
    std::string getreplaces() const;
    std::string getrepo() const;
    std::string getsignature() const;
    std::string getsize() const;
    std::string getupdatestate() const;
    std::string geturl() const;
    std::string getversion() const;

    std::string getattr(AttributeEnum attr) const;
    off_t getoffattr(AttributeEnum attr) const;

    void setcolindex(int index);
    int getcolindex() const;

    void setop(OperationEnum oe);
    OperationEnum getop() const;

private:

    std::string trimstr(const char *c) const;
    std::string deplist2str(alpm_list_t *l, std::string delim) const;
    std::string list2str(alpm_list_t *l, std::string delim) const;
    static std::string size2str(off_t size);

    std::string _name,
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
        _signature,
        _installsizestr,
        _localversion;

    int _colindex;

    off_t _size,
          _installsize;

    time_t _builddate;

    UpdateStateEnum _updatestate;

    InstallReasonEnum _reason;

    OperationEnum op;
};

#endif // PACKAGE_H
