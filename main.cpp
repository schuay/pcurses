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

#include <unistd.h>

#include "program.h"
#include "globals.h"

void parseargs(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "hv")) != -1) {
        switch (opt) {
        case 'v':
            fprintf(stdout, "%s %d.%d\n", APPLICATION_NAME, VERSION_MAJOR, VERSION_MINOR);
            exit(EXIT_SUCCESS);
            break;
        case 'h':
        default:
            fprintf(stderr, "Usage: %s [-h] [-v]\n", APPLICATION_NAME);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {

    parseargs(argc, argv);

    Program *p = new Program();

    try {
        p->init();
        p->mainloop();
    } catch (PcursesException e) {
        std::cerr << e.getmessage() << std::endl;
    } catch (...) {
        std::cerr << "guru meditation" << std::endl;
    }

    delete p;

    return 0;
}
