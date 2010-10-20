/***************************************************************************

    Copyright 2010 Jakob Gruber

    This file is part of dungeons.

    dungeons is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    dungeons is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with dungeons.  If not, see <http://www.gnu.org/licenses/>.

 ***************************************************************************/

#include "package.h"

Package::Package(pmpkg_t* pkg) :
        _pkg(NULL),
        _localpkg(NULL)
{
    _pkg = pkg;
    _localpkg = alpm_db_get_pkg(alpm_option_get_localdb(), name().c_str());
}

string Package::name() const
{
    return alpm_pkg_get_name(_pkg);
}
string Package::desc() const
{
    const char *d = alpm_pkg_get_desc(_pkg);
    if (d == NULL) return "";
    return d;
}
string Package::version() const
{
    return alpm_pkg_get_version(_pkg);
}
string Package::dbname() const
{
    return alpm_db_get_name(alpm_pkg_get_db(_pkg));
}
_pmpkgreason_t Package::reason() const
{
    return alpm_pkg_get_reason(_localpkg);
}
string Package::builddate() const {
    time_t t = alpm_pkg_get_builddate(_pkg);
    string timestr = std::ctime(&t);
    return timestr.substr(0, timestr.length() - 1);
}
bool Package::installed() const
{
    return _localpkg != NULL;
}
bool Package::needsupdate() const
{
    if (_localpkg == NULL) return false;
    return alpm_pkg_vercmp(alpm_pkg_get_version(_pkg), alpm_pkg_get_version(_localpkg)) > 0;
}
