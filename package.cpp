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

#include "package.h"

Package::Package(pmpkg_t* pkg) :
        _pkg(NULL),
        _localpkg(NULL)
{
    _pkg = pkg;
    _localpkg = alpm_db_get_pkg(alpm_option_get_localdb(), alpm_pkg_get_name(_pkg));

    _name = char2str(alpm_pkg_get_name(_pkg));
    _url = char2str(alpm_pkg_get_url(_pkg));
    _packager = char2str(alpm_pkg_get_packager(_pkg));
    _desc = char2str(alpm_pkg_get_desc(_pkg));
    _version = char2str(alpm_pkg_get_version(_pkg));
    _dbname = char2str(alpm_db_get_name(alpm_pkg_get_db(_pkg)));
    _builddate = alpm_pkg_get_builddate(_pkg);

    _reason = ((_localpkg == NULL) ? IRE_NOTINSTALLED :
               (alpm_pkg_get_reason(_localpkg) == PM_PKG_REASON_DEPEND) ? IRE_ASDEPS :
               IRE_EXPLICIT);
}

string Package::char2str(const char *c) const {
    if (c == NULL) return "";

    string str = c;

    /* trim string */

    size_t startpos = str.find_first_not_of(" \t\n");
    size_t endpos = str.find_last_not_of(" \t\n");

    if(( string::npos == startpos ) || ( string::npos == endpos))
        return "";
    else
        return str.substr( startpos, endpos-startpos+1 );
}

bool Package::matches(const Package *a, const std::string needle, const std::string op) {
    bool found = false;

    if (op == "n/" || op == "/") {
        found = found || a->name().find(needle) != std::string::npos;
    }
    if (op == "d/" || op == "/") {
        found = found || a->desc().find(needle) != std::string::npos;
    }
    return !found;
}
bool Package::cmp(const Package *lhs, const Package *rhs) {
    return lhs->name() < rhs->name();
}
string Package::name() const
{
    return _name;
}
string Package::desc() const
{
    return _desc;
}
string Package::version() const
{
    return _version;
}
string Package::dbname() const
{
    return _dbname;
}
InstallReasonEnum Package::reason() const
{
    return _reason;
}
string Package::reasonstring() const {
    switch (_reason) {
    case IRE_NOTINSTALLED:
        return "not installed";
    case IRE_EXPLICIT:
        return "explicit";
    case IRE_ASDEPS:
        return "as dependency";
    default:
        throw AlpmException("no package install reason.");
    }
}
string Package::packager() const {
    return _packager;
}
string Package::url() const {
    return _url;
}
string Package::builddate() const {
    string timestr = std::ctime(&_builddate);
    return timestr.substr(0, timestr.length() - 1); //remove newline
}
bool Package::needsupdate() const
{
    if (_localpkg == NULL) return false;
    return alpm_pkg_vercmp(alpm_pkg_get_version(_pkg), alpm_pkg_get_version(_localpkg)) > 0;
}
void Package::setop(OperationEnum oe) {
    op = oe;
}
OperationEnum Package::getop() const {
    return op;
}
