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
 * WorkerThread.cpp
 *  Created on: Apr 28, 2011
 */

#include <cstdio>	// perror()
#include <cstdlib>	// EXIT_FAILURE
#include <climits>	// PIPE_BUF
#include <fstream>
#include <iostream>
#include <sstream>

#include <errno.h>	// errno
#include <fcntl.h>	// open(), fcntl()
#include <signal.h> // kill()
#include <sys/resource.h> // setrlimit()
#include <sys/types.h>	// fork(), open()
#include <sys/wait.h> // waitpid()
#include <unistd.h>	// pipe(), close(), fork(), dup2(), execv(), fcntl()
#include "WorkerThread.h"

namespace quickly {

const char * POPEN2_MSGS[] = {"", // 0
								"Call to pipe() failed.", // -1
								"Call to fork() failed.", // -2
								"Call to fcntl() using F_GETFL failed.", // -3
								"Attempt to use non-blocking reads failed.", // -4
								};

/*
 * Forks a new process and connects its standard output to the parent's
 * standard input, then runs execv() to run the child. 
 *
 * Returns the child's
 * PID when OK or a negative number if an error is encountered.
 * Inspired by http://snippets.dzone.com/posts/show/1134
 */
pid_t popen2(const char *proc, const char * const *argv, int *outfp, unsigned int vm_lim = 0U, unsigned int CPU_lim = 0U) {
	const int READ = STDIN_FILENO;
	const int WRITE = STDOUT_FILENO;
	const int ERR = STDERR_FILENO;
	int p_stdout[2];
	pid_t pid;

	// Create a pipe
	if (pipe(p_stdout) == -1)
		return -1;

	pid = fork();

	if (pid == -1) {
		// Fork failed
		close(p_stdout[READ]);
		close(p_stdout[WRITE]);
		return -2;
	} else if (pid == 0) {
		// Child process
		// Don't read from stdout
		close(p_stdout[READ]);
		// Pipe child's stdout to parent's stdin
		if (dup2(p_stdout[WRITE], WRITE) == -1) {
			std::exit(EXIT_FAILURE);
		}

		// Pipe stderr to /dev/null
		int std_err = open("/dev/null", O_WRONLY);
		if (std_err == -1) {
			std::exit(EXIT_FAILURE);
		}
		if (dup2(std_err, ERR) == -1) {
			std::exit(EXIT_FAILURE);
		}

		/* In order to use the argv parameter, which is of type
		 * "const char * const *" with execv, which accepts a "char * const *",
		 * we must use a const_cast. It is safe to use it here, as is discussed
		 * in http://stackoverflow.com/questions/190184/execv-and-const-ness */
		char * const *argv_nonconst = const_cast<char * const *>(argv);

		if (vm_lim != 0U) {
			// Limit virtual memory size
			struct rlimit rl;
			rl.rlim_cur = vm_lim;
			rl.rlim_max = vm_lim;
			if (setrlimit(RLIMIT_AS, &rl) == -1) {
				std::exit(EXIT_FAILURE);
			}
		}
		if (CPU_lim != 0) {
			// Limit CPU time
			struct rlimit rl;
			rl.rlim_cur = CPU_lim;
			rl.rlim_max = CPU_lim;
			if (setrlimit(RLIMIT_CPU, &rl) == -1) {
				std::exit(EXIT_FAILURE);
			}
		}

		// Replace process image
		execv(proc, argv_nonconst);
		// Replace failed, show error and exit
		std::perror("execv");
		std::exit(EXIT_FAILURE);
	} else {
		// Parent process
		// Return a handle to the newly created pipe
		*outfp = p_stdout[READ];
		// Don't write to the new pipe
		close(p_stdout[WRITE]);
		// Use non-blocking reads
		int flags = fcntl(p_stdout[READ], F_GETFL, 0);
		if (flags == -1) {
			close(p_stdout[READ]);
			perror("fcntl F_GETFL");
			return -3;
		}

		if (fcntl(p_stdout[READ], F_SETFL, flags | O_NONBLOCK) == -1) {
			close(p_stdout[READ]);
			perror("fcntl F_SETFL");
			return -4;
		}
		return pid;
	}
}

void WorkerThread::message(const char *message) {
	boost::mutex::scoped_lock lock(print_mutex);
	std::cerr << "Thread " << boost::this_thread::get_id() << ": " << message
			<< std::endl;
}

void WorkerThread::operator ()(void) {
	if (child_params.getChildProc() == 0) {
		message("Child process name not set.");
	}
	if (child_params.getArgv() == 0) {
		message("Child process arguments not set.");
		return;
	}
	if (id == (unsigned int) -1) {
		message("Result id not set.");
		return;
	}

	// Initialize a new data action
	DataActionBase *action = data_action->create(id);

	// Runs a new instance of the child process
	int fd;
	const pid_t PID = popen2(child_params.getChildProc(), child_params.getArgv(),
			&fd, child_params.getVmLimit(), child_params.getCpuLimit());
	if (PID < 0) {
		message(POPEN2_MSGS[-PID]);
		delete action;
		return;
	}
	
	// Fill a buffer with the data output from the child process.
	std::stringstream buffer;
	char read_buf[PIPE_BUF];
	size_t nbytes = sizeof(read_buf);
	ssize_t bytes_read;
	while (true) {
		bytes_read = read(fd, read_buf, nbytes);
		if (bytes_read > 0) { // Success
			buffer.write(read_buf, bytes_read);
		} else if (bytes_read == 0) { // EOF
			close(fd);
			int status = 0;
			int r = waitpid(PID, &status, 0);
			if (r != PID or not WIFEXITED(status)) { // Problematic child
				std::string errmsg("child process killed: ");
				for (int ei = 0; child_params.getArgv()[ei] != NULL; ei++) {
					errmsg += child_params.getArgv()[ei];
					errmsg += " ";
				}
				message(errmsg.c_str());
				kill(PID, SIGKILL);
				delete action;
				return;
			} else { // Done reading
				// Run the doFull action with the buffered data
				action->doFull(buffer);
				delete action;
				break;
			}
		} else if (bytes_read == -1 && errno == EAGAIN) { // Empty pipe
			static const boost::posix_time::time_duration timeout =
					boost::posix_time::milliseconds(10);
			boost::this_thread::sleep(timeout);
		} else { // Error
			message("read() error");
			close(fd);
			waitpid(PID, NULL, 0);
			delete action;
			return;
		}
	}
}
}
