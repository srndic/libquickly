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
 * ThreadPool.cpp
 *  Created on: Apr 29, 2011
 */

#include <iostream>

#include <boost/date_time.hpp>
#include <boost/thread.hpp>

#include "ChildParams.h"
#include "ThreadPool.h"

namespace quickly {

bool ThreadPool::run() {
	if (verbosity > 0) {
		std::cerr << "ThreadPool running with " << CHILD_COUNT << " threads." << std::endl;
	}
	// Index of the next job/thread to start
	unsigned int next_job = 0;
	// Number of finished jobs/threads
	unsigned int jobs_done = 0;
	// Primitive Boost thread pool
	boost::thread_group threads;
	// Pointers to the threads in the thread pool. Needed to reference them
	boost::thread **tps = new boost::thread*[CHILD_COUNT];
	for (unsigned int i = 0; i < CHILD_COUNT; i++) {
		tps[i] = (boost::thread *) NULL;
	}
	// A cached value for a 0-millisecond thread sleep timeout
	static const boost::posix_time::time_duration timeout =
			boost::posix_time::milliseconds(0);

	// Go through all jobs to be done
	while (jobs_done < child_args.size()) {
		/*
		 * Start a new job/thread if the number of concurrently running threads
		 * is not at its maximum and if all jobs have not yet been started
		 */
		if (threads.size() < CHILD_COUNT && next_job < child_args.size()) {
			// Find the first unused (NULL) slot in the thread pool
			unsigned int tokbufi = std::find(tps, tps + CHILD_COUNT,
					(boost::thread *) NULL) - tps;
			// Create a new thread and child process parameters
			ChildParams params(child_proc, child_args[next_job], VM_limit, CPU_limit);
			WorkerThread worker;
			worker.init(params, data_action, next_job);
			next_job++;
			// Start the new thread
			boost::thread *thread = threads.create_thread(worker);
			tps[tokbufi] = thread;
		}

		/*
		 * Wait for a thread to stop if the maximum number of concurrently
		 * running threads has been reached, or there are no more
		 * jobs/threads to start
		 */
		if (threads.size() == CHILD_COUNT || next_job == child_args.size()) {
			// Poll all the threads in the pool until at least one thread
			// voluntarily finishes
			unsigned int i = 0;
			for (;; i = (i + 1) % CHILD_COUNT) {
				if (tps[i] != (boost::thread *) NULL
						&& tps[i]->timed_join(timeout)) {
					break;
				}
			}

			// Deallocate the finished thread
			threads.remove_thread(tps[i]);
			delete tps[i];
			tps[i] = (boost::thread *) NULL;
			jobs_done++;
			unsigned int jobs_remaining = child_args.size() - jobs_done;
			if (verbosity > 1) {
				if (jobs_remaining % 100 == 0 || jobs_remaining < 100) {
					std::cerr << child_args.size() - jobs_done << std::endl;
				}
			}
		}
	}

	// Deallocate memory
	delete[] tps;

	if (verbosity > 2) {
		std::cerr << "ThreadPool finished." << std::endl;
	}
	return true;
}

}
