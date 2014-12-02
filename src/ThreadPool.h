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
 * ThreadPool.h
 *  Created on: Apr 29, 2011
 */

#ifndef QUICKLY_THREADPOOL_H_
#define QUICKLY_THREADPOOL_H_

#include <algorithm> // max()
#include <vector>

#include "DataAction.h"
#include "WorkerThread.h"

namespace quickly {
/*!
 * \brief A class representing a pool of worker threads.
 *
 * The number of threads is a parameter of the constructor and is kept constant over time.
 */
class ThreadPool {
private:
	// Name of the child executable
	const char *child_proc;
	// Arguments to the child processes
	std::vector<const char * const *> child_args;
	// An action to perform on the results obtained from the child processes
	DataActionBase *data_action;
	// Maximum number of processes to run concurrently
	unsigned int CHILD_COUNT;
	// Virtual memory limit (in bytes) for child processes
	unsigned int VM_limit;
	// CPU time limit (in seconds) for child processes
	unsigned int CPU_limit;
	// Level of verbosity
	unsigned int verbosity;
public:
	/*!
	 * \brief Constructor
	 *
	 * \param child_proc path and name of the executable to execute.
	 * \param child_args a vector whose elements are command-line arguments to be passed to the executable. The first argument is always the name of the executable (without a path).
	 * \param data_action an instance of a class inheriting DataActionBase which contains the code that will process the results returned by the child executables.
	 * \param child_count the maximum number of concurrent threads/executables to run. If 0 (default), it will be set to the number one less than the number of execution pipelines (CPU cores or HyperThreading units) available on the machine.
	 */
	ThreadPool(const char *child_proc,
			const std::vector<const char * const *> &child_args,
			DataActionBase *data_action, unsigned int child_count = 0) :
			child_proc(child_proc), child_args(child_args),
			data_action(data_action), CHILD_COUNT(child_count),
			VM_limit(0U), CPU_limit(0U), verbosity(0U) {
		if (this->child_proc == 0) {
			throw "ThreadPool: Child executable name not set.";
		}
		if (this->child_args.size() < 1) {
			throw "ThreadPool: There must be at least one set of arguments.";
		}
		if (this->CHILD_COUNT == 0) {
			this->CHILD_COUNT = std::max(
					boost::thread::hardware_concurrency() - 1, 1U);
		}
	}

	/*!
	 * \brief Runs the thread pool until all threads finish. Returns true on success,
	 * otherwise false.
	 */
	bool run();

	/*!
	 * \brief Limits the amount of virtual memory every child process can use.
	 *
	 * \param limit maximum amount of virtual memory allowed, in bytes.
	 */
	void setVmLimit(unsigned int limit) {
		VM_limit = limit;
	}
	/*!
	 * \brief Limits the CPU time for every child process.
	 *
	 * \param limit maximum CPU time allowed, in seconds.
	 */
	void setCpuLimit(unsigned int limit) {
		CPU_limit = limit;
	}
	/*!
	 * Set the output verbosity level.
	 *
	 * @param verbosity the new verbosity level.
	 */
	void setVerbosity(unsigned int verbosity) {
		this->verbosity = verbosity;
	}
};

}

#endif /* QUICKLY_THREADPOOL_H_ */
