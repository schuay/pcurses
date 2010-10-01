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
