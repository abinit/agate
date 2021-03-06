/**
 * @file include/ddbabinit.hpp
 *
 * @brief Read an abinit DDB
 *
 * @author Jordan Bieder <jordan.bieder@cea.fr>
 *
 * @copyright Copyright 2016 Jordan Bieder
 *
 * This file is part of Agate.
 *
 * Agate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Agate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Agate.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef DDBABINIT_HPP
#define DDBABINIT_HPP

#ifdef _WIN32
#include "base/win32.hpp"
#endif

#ifdef HAVE_CONFIG_H
#include "agate.h"
#undef HAVE_CONFIG_H
#endif

#include "io/ddb.hpp"

/** 
 * Read a _DDB file and store all the second derivative information 
 */
class DdbAbinit : public Ddb {

  private :

    /**
     * Function to read one block from a DDB file
     * @param idd ifstream to the file to read
     * @param return true if it is a 2nd derivative, false otherwise
     */
    bool readBlock(std::ifstream& idd);


  protected :

  public :

    /**
     * Constructor.
     */
    DdbAbinit();

    /**
     * Destructor.
     */
    virtual ~DdbAbinit();

    /**
     * Fill a DDB with the content of a file.
     * @param filename The name of the input file to read.
     */
    virtual void readFromFile(const std::string& filename);

    static void header(const Ddb &ddb, std::ostream &out);
    static void dump(const Ddb &ddb, std::string filename);
};

#endif  // DDBABINIT_HPP
