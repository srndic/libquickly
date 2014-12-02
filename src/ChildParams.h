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
 * ChildParams.h
 *  Created on: Nov 22, 2013
 */

#ifndef CHILDPARAMS_H_
#define CHILDPARAMS_H_

namespace quickly {

/*
 * A class describing parameters for running a child process.
 */
class ChildParams {
private:
	// Process full name with path
	const char *child_proc;
	// The command-line arguments for the child process
	const char * const *argv;
	// Virtual memory limit (in bytes) for child processes
	unsigned int VM_limit;
	// CPU time limit (in seconds) for child processes
	unsigned int CPU_limit;
public:
	ChildParams() :
			child_proc(0), argv(0), VM_limit(0U), CPU_limit(0U) {
	}
	// Constructor
	ChildParams(const char *child_proc, const char * const *argv,
			unsigned int VM_limit = 0U, unsigned int  CPU_limit = 0U) :
			child_proc(child_proc), argv(argv), VM_limit(VM_limit), CPU_limit(CPU_limit) {
	}
	// Nothing to destruct
	virtual ~ChildParams() {
	}
	const char *getChildProc() const {
		return child_proc;
	}
	const char * const *getArgv() const {
		return argv;
	}
	unsigned int getVmLimit() const {
		return VM_limit;
	}
	unsigned int getCpuLimit() const {
		return CPU_limit;
	}
};

} /* namespace quickly */
#endif /* CHILDPARAMS_H_ */
