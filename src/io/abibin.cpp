/**
 * @file src/./abibin.cpp
 *
 * @brief 
 *
 * @author Jordan Bieder <jordan.bieder@cea.fr>
 *
 * @copyright Copyright 2018 Jordan Bieder
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


#include "io/abibin.hpp"
#include "base/exception.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>

const std::vector<int> AbiBin::densityFform({52,53,54,55,56,57,58,59,60,61,62,63,67,68,69,110,70,64,65,66,71});
const std::vector<int> AbiBin::potentialFform({102,103,104,105,106,107,108,109,111,112,113,114});

//
AbiBin::AbiBin() : Dtset(),
  _nsppol(0),
  _ngfft{0,0,0},
  _fftData()
{
  ;
}

//
AbiBin::~AbiBin() {
  ;
}

void AbiBin::readFromFile(const std::string& filename) {
  std::ifstream file;
  
  file.open(filename,std::ios::in|std::ios::binary);

  if ( !file )
    throw EXCEPTION(std::string("File ")+filename+" could not be opened",ERRABT);

  unsigned int marker;                  // Size of the next variable.

  std::vector<int> isize;
  int ifullsize;
  std::vector<int> dsize;
  int dfullsize;
  std::vector<int> idummy;
  std::vector<double> ddummy;

  auto checkMarker = [&](int level, int charsize=0){
    file.read((char*)(&marker),sizeof(int));
    if ( level > 0 ) {
      ifullsize = std::accumulate(isize.begin(),isize.end(),0);
      dfullsize = std::accumulate(dsize.begin(),dsize.end(),0);
    }
    /*
    std::clog << marker << std::endl;
    std::clog << charsize*sizeof(char)<< std::endl;
    std::clog << ifullsize*sizeof(int) << std::endl;
    std::clog << dfullsize*sizeof(double) << std::endl;
    */
    if ( marker != (charsize*sizeof(char)+ifullsize*sizeof(int)+ (dfullsize)*sizeof(double)) )
      throw EXCEPTION(std::string("Bad header file: <")+(level<0?"/H":"H")+utils::to_string(std::abs(level))+">",ERRABT);
    if ( level > 0 ) {
      idummy.resize(*std::max_element(isize.begin(),isize.end()));
      ddummy.resize(*std::max_element(dsize.begin(),dsize.end()));
    }
  };

 /**
  * FIRST RECORD
  * read(unit, err=10, iomsg=errmsg) hdr%codvsn,hdr%headform,fform
  */
 char codvsn[7]; //7 is \0 only 6 relevant char.
 int headform;   // Format of the header
 int fform;      // Code for data inside this file. Here it should be density or potential fform

  file.read((char*)(&marker),sizeof(int));
  if ( marker != (6*sizeof(char)+2*sizeof(int)) )
    throw EXCEPTION("Bad header file: <H1>",ERRDIV);

  file.read(codvsn,6);
  codvsn[6]='\0';
  file.read((char*)&headform,sizeof(int));
  file.read((char*)&fform,sizeof(int));

  file.read((char*)(&marker),sizeof(int));
  if ( marker != (6*sizeof(char)+2*sizeof(int)) )
    throw EXCEPTION("Bad header file: </H1>",ERRABT);

  // Check codvsn
  /*
  std::clog << "Abinit unformatted file version " << codvsn << std::endl;
  std::clog << "Header format " << headform << std::endl;
  */
  if ( std::find(&densityFform[0],&densityFform[0]+densityFform.size(),fform) )
    std::clog << "Density file" << std::endl;
  else if ( std::find(&potentialFform[0],&potentialFform[0]+potentialFform.size(),fform) )
    std::clog << "Potential file" << std::endl;

 /**
  * SECOND RECORD
  * read(unit, err=10, iomsg=errmsg) &
  * &  hdr%bantot, hdr%date, hdr%intxc, hdr%ixc, hdr%natom, hdr%ngfft(1:3),&
  * &  hdr%nkpt, hdr%nspden, hdr%nspinor, hdr%nsppol, hdr%nsym, hdr%npsp, hdr%ntypat, hdr%occopt, hdr%pertcase,&
  * &  hdr%usepaw, hdr%ecut, hdr%ecutdg, hdr%ecutsm, hdr%ecut_eff, hdr%qptn(1:3), hdr%rprimd,&
  * &  hdr%stmbias, hdr%tphysel, hdr%tsmear, hdr%usewvl, hdr%nshiftk_orig, hdr%nshiftk, hdr%mband
  */
  isize = {8,9,1,4};  //Not really consitent but too many variables !
  dsize = {16,3};
  checkMarker(2);

  int bandtot, nkpt, nsym, npsp, nshiftk_orig, nshiftk, mband;
  file.read((char*)(&idummy[0]),4*sizeof(int));
  bandtot = idummy[0];
  file.read((char*)(&_natom),sizeof(int));
  file.read((char*)(_ngfft),3*sizeof(int));

  file.read((char*)(&idummy[0]),3*sizeof(int));
  nkpt = idummy[0];
  file.read((char*)(&_nsppol),sizeof(int));
  file.read((char*)(&idummy[0]),5*sizeof(int));
  nsym = idummy[0];
  npsp = idummy[1];
  _ntypat = idummy[2];

  file.read((char*)(&idummy[0]),1*sizeof(int));
  file.read((char*)(&ddummy[0]),16*sizeof(double));
  int usepaw = idummy[0];
  _rprim[0] = ddummy[7];
  _rprim[3] = ddummy[8];
  _rprim[6] = ddummy[9];
  _rprim[1] = ddummy[10];
  _rprim[4] = ddummy[11];
  _rprim[7] = ddummy[12];
  _rprim[2] = ddummy[13];
  _rprim[5] = ddummy[14];
  _rprim[8] = ddummy[15];
  _acell[0] = 1;
  _acell[1] = 1;
  _acell[2] = 1;
  _gprim = geometry::invertTranspose(_rprim);

  file.read((char*)(&ddummy[0]),3*sizeof(double));
  file.read((char*)(&idummy[0]),4*sizeof(int));
  nshiftk_orig = idummy[1];
  nshiftk = idummy[2];
  mband = idummy[3];

  /*
  std::clog << _natom << " atoms" << std::endl;
  std::clog << _ntypat << " types of atoms" << std::endl;
  std::clog << _nsppol << " spin polarization" << std::endl;
  std::clog << "ngfft is " << _ngfft[0] << " " << _ngfft[1] << " " << _ngfft[2] << std::endl;
  std::clog << "usepaw " << usepaw << std::endl;
  geometry::print(_rprim);
  */

  checkMarker(-2);

  _znucl.resize(_ntypat);
  _typat.resize(_natom);
  _xcart.resize(3*_natom);
  _xred.resize(3*_natom);


  /**
   * THIRD RECORD
   * read(unit, err=10, iomsg=errmsg) &
   * & hdr%istwfk(:), hdr%nband(:), hdr%npwarr(:), &
   * & hdr%so_psp(:), hdr%symafm(:), hdr%symrel(:,:,:), &
   * & hdr%typat(:), hdr%kptns(:,:), occ3d, &
   * & hdr%tnons(:,:), hdr%znucltypat(:), hdr%wtk(:)
   */

  isize = {nkpt,nkpt*_nsppol,nkpt,npsp,nsym,3*3*nsym,(int)_natom}; 
  dsize = {3*nkpt,mband*nkpt*_nsppol,3*nsym,(int)_ntypat,nkpt};
  checkMarker(3);

  for ( int v = 0 ; v < 6 ; ++v )
    file.read((char*)(&idummy[0]),isize[v]*sizeof(int));

  // Read _typat
  file.read((char*)(&_typat[0]),isize[6]*sizeof(int));

  for ( int v = 0 ; v < 3 ; ++v )
    file.read((char*)(&ddummy[0]),dsize[v]*sizeof(double));

  file.read((char*)(&ddummy[0]),dsize[3]*sizeof(double));
  for ( unsigned t = 0 ; t < _ntypat ; ++t )
    _znucl[t] = (unsigned) ddummy[t];

  file.read((char*)(&ddummy[0]),dsize[4]*sizeof(double));

  checkMarker(-3);

  /**
   * FORTH RECORD
   * read(unit, err=10, iomsg=errmsg) hdr%residm, hdr%xred(:,:), hdr%etot, hdr%fermie, hdr%amu(:)
   */
  isize = {};
  dsize = {1,3*(int)_natom,1,1,(int)_ntypat};

  checkMarker(4);

  file.read((char*)(&ddummy[0]),dsize[0]*sizeof(double));
  file.read((char*)(&ddummy[0]),dsize[1]*sizeof(double));
  for ( int a = 0 ; a < _natom ; ++a )
    for(int d = 0 ; d < 3 ; ++d )
      _xred[a][d]=ddummy[a*3+d];
  geometry::changeBasis(_rprim, _xcart, _xred, false);

  for ( int v = 2 ; v < 5 ; ++v )
    file.read((char*)(&ddummy[0]),dsize[v]*sizeof(double));

  checkMarker(-4);

  /**
   * FIFTH RECORD
   * read(unit, err=10, iomsg=errmsg)&
   * hdr%kptopt,hdr%pawcpxocc,hdr%nelect,hdr%charge,hdr%icoulomb,&
   * hdr%kptrlatt,hdr%kptrlatt_orig, hdr%shiftk_orig,hdr%shiftk
   */
  isize = {1,1,1,9,9};
  dsize = {1,1,3*nshiftk_orig,3*nshiftk};
  checkMarker(5);
  for ( int v = 0 ; v < isize.size() ; ++v )
    file.read((char*)(&idummy[0]),isize[v]*sizeof(int));
  for ( int v = 0 ; v < dsize.size() ; ++v )
    file.read((char*)(&ddummy[0]),dsize[v]*sizeof(double));
  checkMarker(-5);

  /**
   * SIXTH RECORD
   * ! Reading the records with psp information ---------------------------------
   * do ipsp=1,hdr%npsp
   * read(unit, err=10, iomsg=errmsg) &
   * &   hdr%title(ipsp), hdr%znuclpsp(ipsp), hdr%zionpsp(ipsp), hdr%pspso(ipsp), hdr%pspdat(ipsp), &
   * &   hdr%pspcod(ipsp), hdr%pspxc(ipsp), hdr%lmn_size(ipsp), hdr%md5_pseudos(ipsp)
   * end do
   */
  isize = {1,1,1,1,1};
  dsize = {1,1};
  for ( int ipsp = 0 ; ipsp < npsp ; ++ipsp ) {
    checkMarker(6*10+ipsp,132+32);
    char tmp[132];
    file.read(tmp,132*sizeof(char));
    for ( int v = 0 ; v < isize.size() ; ++v )
      file.read((char*)(&idummy[0]),isize[v]*sizeof(int));
    for ( int v = 0 ; v < dsize.size() ; ++v )
      file.read((char*)(&ddummy[0]),isize[v]*sizeof(double));
    file.read(tmp,32*sizeof(char));
    checkMarker(-(6*10+ipsp),132+32);
  }


  /*
     if (hdr%usepaw==1) then ! Reading the Rhoij tab if the PAW method was used.
     call pawrhoij_io(hdr%pawrhoij,unit,hdr%nsppol,hdr%nspinor,hdr%nspden,hdr%lmn_size,hdr%typat,hdr%headform,"Read")
     end if
     */


}