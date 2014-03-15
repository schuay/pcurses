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

#include <iostream>
#include <unistd.h>

#include "globals.h"
#include "pcursesexception.h"
#include "program.h"

static char* opt_pcursesconffile = NULL;

void parseargs(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "hvf:")) != -1) {
        switch (opt) {
        case 'f':
            opt_pcursesconffile = optarg;
            break;
        case 'v':
            fprintf(stdout, "%s %d\n", APPLICATION_NAME, VERSION);
            exit(EXIT_SUCCESS);
            break;
        case 'h':
        default:
            fprintf(stderr, "Usage: %s [-h] [-v] [-f %s.conf]\n", APPLICATION_NAME, APPLICATION_NAME);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    std::string err;

    parseargs(argc, argv);

    Program *p = new Program();

    try {
        p->init(opt_pcursesconffile);
        p->mainloop();
    } catch (PcursesException e) {
        err = e.getmessage();
    } catch (...) {
        err = "guru meditation";
    }

    delete p;

    /* printing any errors at this point, because the Program destructor
      clears the screen */
    if (!err.empty()) {
        std::cerr << err << std::endl;

    }

    return 0;
}
