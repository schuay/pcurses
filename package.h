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

#ifndef PACKAGE_H
#define PACKAGE_H

#include <alpm.h>
#include <string>
#include <ctime>

typedef struct __pmpkg_t pmpkg_t;

class Package
{
public:
    Package(pmpkg_t *pkg);

    std::string name() const { return alpm_pkg_get_name(_pkg); }
    std::string desc() const { const char *d = alpm_pkg_get_desc(_pkg); if (d == NULL) return ""; return d; }
    std::string version() const { return alpm_pkg_get_version(_pkg); }
    std::string dbname() const { return alpm_db_get_name(alpm_pkg_get_db(_pkg)); }
    std::string reason() const { return ( alpm_pkg_get_reason(_pkg) == PM_PKG_REASON_EXPLICIT ? "explicit" : "as dependency" ); }
    std::string builddate() const {
        time_t t = alpm_pkg_get_builddate(_pkg);
        std::string timestr = std::ctime(&t);
        return timestr.substr(0, timestr.length() - 1); }
    bool installed() const { return _localpkg != NULL; }
    bool needsupdate() const { if (_localpkg == NULL) return false;
        return alpm_pkg_vercmp(alpm_pkg_get_version(_pkg), alpm_pkg_get_version(_localpkg)) > 0; }
private:
    pmpkg_t
            *_pkg,
            *_localpkg;
};

#endif // PACKAGE_H
