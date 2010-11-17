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

#include "alpmexception.h"

typedef struct __pmpkg_t pmpkg_t;

using std::string;
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

enum AttributeEnum {
    A_NAME,
    A_VERSION,
    A_URL,
    A_REPO,
    A_PACKAGER,
    A_BUILDDATE,
    A_INSTALLSTATE,
    A_DESC,
    A_NONE,
};

class Package
{
public:
    Package(pmpkg_t *pkg);

    string name() const;
    string url() const;
    string packager() const;
    string desc() const;
    string version() const;
    string dbname() const;
    InstallReasonEnum reason() const;
    string reasonstring() const;
    string builddate() const;
    bool needsupdate() const;

    string getattr(AttributeEnum attr) const;

    void setop(OperationEnum oe);
    OperationEnum getop() const;

private:

    string char2str(const char *c) const;

    pmpkg_t
            *_pkg,
            *_localpkg;

    string
            _name,
            _url,
            _packager,
            _desc,
            _version,
            _dbname;

    time_t
            _builddate;

    InstallReasonEnum
            _reason;

    OperationEnum
            op;
};

#endif // PACKAGE_H
