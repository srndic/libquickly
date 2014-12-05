/*
 * Copyright 2014 Nedim Srndic, University of Tuebingen
 *
 * This file is part of libquickly.

 * libquickly is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libquickly is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libquickly.  If not, see <http://www.gnu.org/licenses/>.
 *
 * libquickly.cpp
 *  Created on: Jan 18, 2012
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../src/ThreadPool.h"
#include "../src/DataAction.h"

using std::cout;
using std::endl;

/*
 * A sample implementation of a simple data action.
 */
class DataActionImpl: public quickly::DataActionBase {
private:
	// You should always write a constructor that calls the superclass to set
	// the job ID
	explicit DataActionImpl(unsigned int id) :
		DataActionBase(id) {
	}
public:
	// Dummy constructor
	DataActionImpl() :
		DataActionBase(0U) {
	}
	// Virtual constructor
	virtual DataActionImpl *create(unsigned int id) {
		return new DataActionImpl(id);
	}
	// This is the code that handles the data returned from the child
	// process. We will do a simple print to the standard output
	virtual void doFull(std::stringstream &databuf) {
		cout << "Hello World! Printout follows:\n";
		cout << databuf.str();
		cout << "End of data!" << endl;
	}
};

/*
 * Main program (parent executable).
 */
int main() {
	// Construct a vector of command-line arguments
	std::vector<const char * const *> argvs;
	const char * const argv1[] = {"echo", "one", "two", "three", (char *) NULL};
	const char * const argv2[] = {"echo", "2one", "2two", "2three", (char *) NULL};
	argvs.push_back(argv1);
	argvs.push_back(argv2);

	// Prepare the data action
	DataActionImpl dummy;
	quickly::ThreadPool pool("/bin/echo", argvs, &dummy, 0U);
	bool success = pool.run();
	cout << "Success: " << success << endl;

	cout << "\nExiting" << endl;
	return EXIT_SUCCESS;
}
