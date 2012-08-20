//# SetJyGridFT.cc: Implementation of GridFT class
//# Copyright (C) 2012
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
/*
 * SetJyGridFT.cc
 *
 *  Created on: Jun 11, 2012
 *      Author: kgolap
 */
#include <synthesis/MSVis/VisibilityIterator.h>
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicSL/Constants.h>
#include <scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines/SetJyGridFT.h>
#include <scimath/Mathematics/RigidVector.h>
#include <synthesis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/MSVis/VisBuffer.h>
#include <synthesis/MSVis/VisSet.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MatrixIter.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <measures/Measures/UVWMachine.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/Lattices/LCBox.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/Lattices/LatticeFFT.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <scimath/Mathematics/ConvolveGridder.h>
#include <casa/Utilities/CompositeNumber.h>
#include <casa/OS/Timer.h>
#include <casa/sstream.h>
#ifdef HAS_OMP
#include <omp.h>
#endif

namespace casa { //# NAMESPACE CASA - BEGIN
  //  using namespace casa::async;

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define sectdgridjy sectdgridjy_
#endif

extern "C" {
void sectdgridjy(Complex*,
                const Int*,
                const Int*,
		const Int*,
		const Int*,
		const Int*,
		const Complex*,
                const Int*,
		const Int*,
		const Int *,
		const Int *,
		 //support
                const Int*,
		const Int*,
		const Double*,
		const Int*,
		 const Int*,
		 //rbeg, rend, loc, off, phasor
		 const Int*,
		 const Int*,
		 const Int*,
		 const Int*,
		 const Complex*,
		 const Double*);
}
SetJyGridFT::SetJyGridFT(Long icachesize, Int itilesize, String iconvType,
	       	   MPosition mLocation, MDirection mTangent, Float padding,
	       	   Bool usezero, Bool useDoublePrec,const Vector<Double>& frequencyscale, const Vector<Double>& scale):GridFT(icachesize, itilesize, iconvType,
	       			   mLocation, mTangent, padding, usezero,	useDoublePrec), freqscale_p(frequencyscale),
	       			   scale_p(scale) {

  machineName_p="SetJyGridFT";

  
}

  SetJyGridFT::~SetJyGridFT(){
    
  }

SetJyGridFT::SetJyGridFT(const RecordInterface& stateRec)
: GridFT()
{
  // Construct from the input state record
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create gridder: " + error));
  };
}
SetJyGridFT::SetJyGridFT(const SetJyGridFT& other) : GridFT()
{
  machineName_p=("SetJyGridFT");
  operator=(other);
}

SetJyGridFT& SetJyGridFT::operator=(const SetJyGridFT& other)
{
  if(this!=&other) {
    //Do the base parameters
    GridFT::operator=(other);
    freqscale_p.resize();
    freqscale_p=other.freqscale_p;
    scale_p.resize();
    scale_p=other.scale_p;
  }
  return *this;
}

void SetJyGridFT::setScale(const Vector<Double>& freq, const Vector<Double>& scale){
  freqscale_p.resize();
  freqscale_p=freq;
  scale_p.resize();
  scale_p=scale;

}
String SetJyGridFT::name(){

	return String("SetJyGridFT");
}
void SetJyGridFT::initializeToVis(ImageInterface<Complex>& image,
		       const VisBuffer& vb){
	setFreqInterpolation(String("nearest"));
	GridFT::initializeToVis(image, vb);
}

Bool SetJyGridFT::toRecord(String& error,
		      RecordInterface& outRec, Bool withImage)
{
  if(!GridFT::toRecord(error, outRec, withImage))
    return False;
  outRec.define("freqscale", freqscale_p);
  outRec.define("scaleamp", scale_p);
  return True;
}
Bool SetJyGridFT::fromRecord(String& error,
			const RecordInterface& inRec)
{
  
  if(!GridFT::fromRecord(error, inRec))
    return False;
  freqscale_p.resize();
  inRec.get("freqscale", freqscale_p);
  scale_p.resize();
  inRec.get("scaleamp", scale_p);
  machineName_p="SetJyGridFT";
  return True;
}
void SetJyGridFT::get(VisBuffer& vb, Int row){
  //Did somebody really want this version.
  if(scale_p.nelements()==0)
    return GridFT::get(vb,row);

  gridOk(gridder->cSupport()(0));
  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row < 0) {
    nRow=vb.nRow();
    startRow=0;
    endRow=nRow-1;
	    //unnecessary zeroing
	    //vb.modelVisCube()=Complex(0.0,0.0);
	  } else {
	    nRow=1;
	    startRow=row;
	    endRow=row;
	    //unnecessary zeroing
	    //vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
	  }

	  // Get the uvws in a form that Fortran can use
	  Matrix<Double> uvw(3, vb.uvw().nelements());
	  uvw=0.0;
	  Vector<Double> dphase(vb.uvw().nelements());
	  dphase=0.0;
	  //NEGATING to correct for an image inversion problem
	  for (Int i=startRow;i<=endRow;i++) {
	    for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
	    uvw(2,i)=vb.uvw()(i)(2);
	  }
	  rotateUVW(uvw, dphase, vb);
	  refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);



	  //Check if ms has changed then cache new spw and chan selection
	  if(vb.newMS())
	    matchAllSpwChans(vb);


	  //Here we redo the match or use previous match

	  //Channel matching for the actual spectral window of buffer
	  if(doConversion_p[vb.spectralWindow()]){
	    matchChannel(vb.spectralWindow(), vb);
	  }
	  else{
	    chanMap.resize();
	    chanMap=multiChanMap_p[vb.spectralWindow()];
	  }

	  //cerr << "chanMap " << chanMap << endl;
	  //No point in reading data if its not matching in frequency
	  if(max(chanMap)==-1)
	    return;

	  Cube<Complex> data;
	  Cube<Int> flags;
	  getInterpolateArrays(vb, data, flags);

	  Vector<Double> lsrfreq;
	  Bool convert;
	  vb.lsrFrequency(vb.spectralWindow(), lsrfreq, convert);
	  interpscale_p.resize(lsrfreq.nelements());
	  InterpolateArray1D<Double,Double>::
	        interpolate(interpscale_p,lsrfreq, freqscale_p, scale_p,InterpolateArray1D<Double, Double>::nearestNeighbour);



	  //    IPosition s(data.shape());
	  Int nvp=data.shape()(0);
	  Int nvc=data.shape()(1);
	  Int nvisrow=data.shape()(2);


	  //cerr << "get flags " << min(flags) << "  " << max(flags) << endl;
	  Complex *datStorage;
	  Bool isCopy;
	  datStorage=data.getStorage(isCopy);

	  ///
	  Cube<Int> loc(2, nvc, nvisrow);
	  Cube<Int> off(2, nvc, nRow);
	  Matrix<Complex> phasor(nvc, nRow);
	  Int csamp=gridder->cSampling();
	  Bool delphase;
	  Bool del;
	  Complex * phasorstor=phasor.getStorage(delphase);
	  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
	  const Double * scalestor=uvScale.getStorage(del);
	  const Double * offsetstor=uvOffset.getStorage(del);
	  const Double* uvstor= uvw.getStorage(del);
	  Int * locstor=loc.getStorage(del);
	  Int * offstor=off.getStorage(del);
	  const Double *dpstor=dphase.getStorage(del);
	  const Double *freqscalestor=interpscale_p.getStorage(del);
	  //Vector<Double> f1=interpVisFreq_p.copy();
	  Int nvchan=nvc;
	  Int irow;
	#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvchan, scalestor, offsetstor, csamp, phasorstor, uvstor, locstor, offstor, dpstor) shared(startRow, endRow) num_threads(4)
	  {
	#pragma omp for
	  for (irow=startRow; irow<=endRow; ++irow){
	    locateuvw(uvstor,dpstor, visfreqstor, nvchan, scalestor, offsetstor, csamp,
		      locstor,
		      offstor, phasorstor, irow);
	  }

	  }//end pragma parallel

	  Int rbeg=startRow+1;
	  Int rend=endRow+1;


	  Vector<Int> rowFlags(vb.nRow());
	  rowFlags=0;
	  rowFlags(vb.flagRow())=True;
	  if(!usezero_p) {
	    for (Int rownr=startRow; rownr<=endRow; rownr++) {
	      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
	    }
	  }


	  //cerr <<"offset " << min(off) << "  " <<max(off) << " length " << gridder->cFunction().shape() << endl;

	  {
	    Bool delgrid;
	    const Complex* gridstor=griddedData.getStorage(delgrid);
	    const Double * convfuncstor=(gridder->cFunction()).getStorage(del);

	    const Int* pmapstor=polMap.getStorage(del);
	    const Int *cmapstor=chanMap.getStorage(del);
	    Int nc=nchan;
	    Int np=npol;
	    Int nxp=nx;
	    Int nyp=ny;
	    Int csupp=gridder->cSupport()(0);
	    const Int * flagstor=flags.getStorage(del);
	    const Int * rowflagstor=rowFlags.getStorage(del);


	    Int npart=1;
	#ifdef HAS_OMP
	    Int nthreads=omp_get_max_threads();
	    if (nthreads >3){
	      npart=4;
	    }
	    else if(nthreads >1){
	     npart=2;
	    }
	#endif
	    Int ix=0;
	#pragma omp parallel default(none) private(ix, rbeg, rend) firstprivate(datStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc, csamp, csupp, nvp, nvc, nvisrow, phasorstor, locstor, offstor, freqscalestor) shared(npart) num_threads(npart)
	    {
	#pragma omp for nowait
	      for (ix=0; ix< npart; ++ix){
		rbeg=ix*(nvisrow/npart)+1;
		rend=(ix != (npart-1)) ? (rbeg+(nvisrow/npart)-1) : (rbeg+(nvisrow/npart)+nvisrow%npart-1) ;
		//cerr << "rbeg " << rbeg << " rend " << rend << "  " << nvisrow << endl;

		sectdgridjy(datStorage,
			  &nvp,
			  &nvc,
			  flagstor,
			  rowflagstor,
			  &nvisrow,
			  gridstor,
			  &nxp,
			  &nyp,
			  &np,
			  &nc,
			  &csupp,
			  &csamp,
			  convfuncstor,
			  cmapstor,
			  pmapstor,
			  &rbeg, &rend, locstor, offstor, phasorstor, freqscalestor);
	      }//end pragma parallel
	    }
	    data.putStorage(datStorage, isCopy);
	    griddedData.freeStorage(gridstor, delgrid);
	    //cerr << "Get min max " << min(data) << "   " << max(data) << endl;
	  }
	  interpolateFrequencyFromgrid(vb, data, FTMachine::MODEL);


}



}
