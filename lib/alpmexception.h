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

#ifndef ALPMEXCEPTION_H
#define ALPMEXCEPTION_H

#include <alpm.h>
#include <string>

using std::string;

class AlpmException
{
public:
    AlpmException() { _msg = "Unexpected error"; _alpmError = alpm_strerrorlast(); }
    AlpmException(string msg) { _msg = msg; _alpmError = alpm_strerrorlast(); }

    string GetMessage() const { return _msg; }
    string GetAlpmError() const { return _alpmError; }

private:
    string _msg;
    string _alpmError;
};

#endif // ALPMEXCEPTION_H
