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
 * ThreadSafeMap.h
 *  Created on: Apr 29, 2011
 */

#ifndef QUICKLY_THREADSAFEMAP_H_
#define QUICKLY_THREADSAFEMAP_H_

#include <map>
#include <boost/thread.hpp>

namespace quickly {
/*
 * A class representing a thread-safe map. It utilizes a mutex to ensure that
 * no two threads can access it at the same time.
 */
template<typename key_t, typename val_t>
class ThreadSafeMap {
private:
	// The map data structure
	std::map<key_t, val_t> map;
	// The mutex for synchronization
	boost::mutex mutex;

	// The type of items in the map
	typedef std::pair<key_t, val_t> map_item_t;
	// The type of the map iterator
	typedef typename std::map<key_t, val_t>::iterator map_iterator_t;
public:
	// Constructor
	ThreadSafeMap() :
		map() {
	}

	// Inserts an element
	void insert(const key_t &key, const val_t &value);
	// Returns an element with the given key by reference. Returns true if
	// found, false otherwise.
	bool get(const key_t &key, val_t &ret);
	// Returns all key-value pairs
	void getAll(std::map<key_t, val_t> &ret) {
		ret = map;
	}
};

template<typename key_t, typename val_t>
inline void ThreadSafeMap<key_t, val_t>::insert(const key_t &key,
		const val_t &value) {
	// Get access for writing
	boost::mutex::scoped_lock lock(mutex);

	map[key] = value;

	// It seems that sometimes the lock does not get automatically 
	// released?! Is a bug fix required in the library?
	lock.unlock();
}

template<typename key_t, typename val_t>
inline bool ThreadSafeMap<key_t, val_t>::get(const key_t &key, val_t &ret) {
	// Get access for reading
	boost::mutex::scoped_lock lock(mutex);

	static map_iterator_t it;
	it = map.find(key);

	if (it == map.end()) {
		// The element with the specified key does not exist
		// This should never happen
		return false;
	}
	ret = (*it).second;

	// Lock gets released automatically by the destruction of the lock object
	return true;
}
}

#endif /* QUICKLY_THREADSAFEMAP_H_ */
