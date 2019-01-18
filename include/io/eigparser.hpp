/**
 * @file include/eigparser.hpp
 *
 * @brief File to read a _EIG file written by abinit
 *
 * @author Jordan Bieder <jordan.bieder@cea.fr>
 *
 * @copyright Copyright 2014 Jordan Bieder
 *
 * This file is part of AbiOut.
 *
 * AbiOut is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbiOut is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbiOut.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef EIGPARSER_HPP
#define EIGPARSER_HPP

#ifdef _WIN32
#include "base/win32.hpp"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#undef HAVE_CONFIG_H
#endif

#include<string>
#include<vector>
#include "base/phys.hpp"
#include "base/geometry.hpp"
#include "io/dtset.hpp"

/** 
 * Simple class to handle a _EIG file generated by Abinit
 */
class EigParser {

  public :

  protected :
    std::string                        _filename; ///< Name of the _EIG file to read
    std::vector<geometry::vec3d>       _kpts;     ///< List of all kpts
    std::vector<double>                _lengths;  ///< Distance from the first kpt.
    std::vector< std::vector<double> > _eigens;   ///< eigen energies for each kpt.
    unsigned                           _nband;    ///< Number of bands to plot
    Units::Energy                      _eunit;    ///< Energy unit in the _EIG.
    double                             _conversion; ///< Conversion factor from input to _eunit;
    bool                               _hasSpin;  ///< Are we polarized
    std::vector<unsigned>              _ndiv;
    std::vector<std::string>           _labels;

  public :

    static const unsigned PRTKPT  = 1;    ///< Options to print additional informations
    static const unsigned PRTIKPT = 1<<1; ///< options to print additional informations
    static const unsigned PRTPROJ = 1<<2; ///< options to print additional informations

    /**
     * Constructor.
     */
    EigParser();

    /**
     * Destructor.
     */
    virtual ~EigParser();

    /**
     * File to read to get the eigen value (_EIG file from abinit)
     * @param filename Name of the _EIG file
     */
    virtual void readFromFile(const std::string& filename) = 0;

    /**
     * Dump all data in a string as an array readable by all plotter
     * @param options Options to print some additionnal information (kpt|ikpt)
     * @param umask id of atoms that should not be considered if projection is asked
     * @return string with a beautiful array and header line
     */
    std::string dump(unsigned options, std::vector<unsigned> umask = std::vector<unsigned>()) const;

    /**
     * Dump all data in a string as an array readable by all plotter
     * @param out The ostream stream to write into.
     * @param options Options to print some additionnal information (kpt|ikpt)
     * @param umask id of atoms that should not be considered if projection is asked
     */
    void dump(std::ostream& out, unsigned options, std::vector<unsigned> umask = std::vector<unsigned>()) const;

    /**
     * Dump all data in a string as an array readable by all plotter
     * @param filename The name of the file to be written.
     * @param options Options to print some additionnal information (kpt|ikpt)
     * @param umask id of atoms that should not be considered if projection is asked
     */
    void dump(const std::string& filename, unsigned options, std::vector<unsigned> umask = std::vector<unsigned>()) const;

    /**
     * Getter for the number of band
     * @return the number of band for each kpt
     */
    unsigned getNband() const { return _nband; }

    /**
     * Getter for the unit
     * @return the unit of the eigen values;
     */
    Units::Energy getUnit() const { return _eunit; }

    /**
     * Set the new unit for the bands we have
     * @param u is the Unit we want for the energies
     */
    void setUnit(Units::Energy u);

    /**
     * Getter for the spin polarization
     * @return true if we have to spin polarizations and thus 2 times the number of bands
     */
    bool isPolarized() const { return _hasSpin; }

    /**
     * Getter the length of the kpt path.
     * @return the length of the full kpt path (unitless).
     */
    double getLength() const { return _lengths[_lengths.size()-1]; }

    /**
     * Getter the ndiv array of string which is the number of point for each path
     * @return the array of string
     */
    std::vector<unsigned> getNdiv() const { return _ndiv; }

    /**
     * Getter the labels array of string for each path
     * @return the array of labels
     */
    std::vector<std::string> getLabels() const { return _labels; }
    
    /**
     * Getter eigenvalue of the ith band
     * @param iband The band for which we want the eigen values;
     * @param fermi Fermi level to shift the bands
     * @param ispin Spin of the band we want can only be 1 or 2
     * @return the eigen values along the path for the ith band
     */
    std::vector<double> getBand(const unsigned iband, const double fermi, const unsigned ispin) const;

    /**
     * Getter color which represent the atom decomposition of the ith band
     * @param iband The band for which we want the eigen values;
     * @param ispin Spin of the band we want can only be 1 or 2
     * @param mask id of atoms that should not be considered if projection is asked
     * @return the RBG color to be used along this band
     */
    virtual std::vector<unsigned> getBandColor(const unsigned iband, const unsigned ispin, std::vector<unsigned> mask = std::vector<unsigned>()) const = 0;

    /**
     * Getter the kpt-path
     * @return the kpt list
     */
    const std::vector<geometry::vec3d>& getKpts() const { return _kpts; }

    /**
     * Getter the path
     * @return the path 
     */
    std::vector<double> getPath() const { 
      return std::vector<double>(
          _lengths.begin(),
          (_hasSpin ? _lengths.begin()+_lengths.size()/2 : _lengths.end())
          ); }

    /**
     * Getter for the filename\
     * @return the name of the file read for generating this structure
     */
    std::string getFilename() { return _filename; }
    
    /**
     * Try to read a band structure file
     * Can read from abinit _EIG _EIG.nc _PHFRQ band.yaml
     * @param file Name of the file to read
     * @return A pointer to the eigparser or throw an Exception if failed.
     */
    static EigParser* getEigParser(const std::string& file);


};

#endif  // EIGPARSER_HPP
