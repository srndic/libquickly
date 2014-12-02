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
 * WorkerThread.h
 *  Created on: Apr 28, 2011
 */

#ifndef QUICKLY_WORKERTHREAD_H_
#define QUICKLY_WORKERTHREAD_H_

#include <boost/thread.hpp>

#include "ChildParams.h"
#include "DataAction.h"

namespace quickly {
/*
 * A thread that controls a child executable. It spawns a process
 * that runs the child executable, and opens a pipe to read its output.
 *
 * If the child process does not finish within a specified timeout, it gets
 * killed. This thread is supposed to always return normally.
 */
class WorkerThread {
	// Parameters for the child process
	ChildParams child_params;
	// The ID of the result that this thread produces
	unsigned int id;
	// An action to perform on the results obtained from the child processes
	DataActionBase *data_action;
	// A mutex for thread-safe message printing
	mutable boost::mutex print_mutex;

	// Prints a formatted message to stdout using locks to ensure correct
	// printing
	void message(const char *message);
public:
	// Constructor (must have an empty constructor for Boost.Threading)
	WorkerThread() :
			child_params(), id((unsigned int) -1), data_action(
					(DataActionBase *) NULL) {
	}

	/*
	 * Copy constructor needs to be implemented because this class is used
	 * as a thread. However, print_mutex is of type boost::mutex, which is
	 * noncopyable.
	 * More info: http://boost.cppll.jp/BDTJ_1_29/libs/thread/doc/faq.html#question5
	 */
	WorkerThread(const WorkerThread &other) : child_params(other.child_params),
			id(other.id), data_action(other.data_action) {
	}
	// Assignment operator automatic
	// Destructor
	virtual ~WorkerThread() {
	}

	/*
	 * Initializes the thread. This method has to be run before
	 * invoking operator().
	 */
	bool init(ChildParams child_params, DataActionBase *data_action,
			unsigned int id) {
		this->child_params = child_params;
		this->id = id;
		this->data_action = data_action;
		return true;
	}

	// overloaded () operator (for Boost.Threading)
	void operator ()();
};

}

#endif /* QUICKLY_WORKERTHREAD_H_ */
