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
 * DataAction.h
 *  Created on: Jan 18, 2012
 */

#ifndef QUICKLY_DATAACTION_H_
#define QUICKLY_DATAACTION_H_

#include <sstream>	// std::stringstream

namespace quickly {
/*!
 * \brief A class representing an action to perform upon the data that is returned
 * from a child process.
 *
 * This is an abstract base class.
 */
class DataActionBase {
private:
	// A unique job id
	unsigned int id;
protected:
	/*! \brief Constructor.
	 *
	 * Call the constructor to ensure the ID is set.
	 *
	 * \param id a unique job ID.
	 */
	explicit DataActionBase(unsigned int id) :
		id(id) {
	}
	
	/*
	 * \brief Returns the ID.
	 */
	unsigned int getId() {
		return id;
	}
public:
	/*!
	 * \brief A virtual constructor.
	 *
	 * See http://www.parashift.com/c++-faq-lite/virtual-functions.html#faq-20.8
	 */
	virtual DataActionBase *create(unsigned int id) = 0;
	
	/*!
	 * \brief This method gets called when all the data has been recieved from the
	 * child executable.
	 *
	 * \param databuf a stream of the entire data output of the child process.
	 */
	virtual void doFull(std::stringstream &databuf) = 0;
	
	/*!
     * \brief A virtual destructor.
     */
	virtual ~DataActionBase() {}
};

}
#endif /* QUICKLY_DATAACTION_H_ */
