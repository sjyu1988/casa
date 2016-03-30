//# WPConvFunc.h: Definition for WPConvFunc
//# Copyright (C) 2007-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_TRANSFORM2_WPCONVFUNC_H
#define SYNTHESIS_TRANSFORM2_WPCONVFUNC_H


#include <casa/Arrays/Vector.h>
#include <casa/Containers/Block.h>
#include <casa/Utilities/CountedPtr.h>

namespace casa{
	template<class T> class ImageInterface;
	template<class T> class Matrix;
	namespace vi{class VisBuffer2;}

namespace refim{// namespace for imaging refactoring

  // <summary>  A class to support FTMachines get their convolution Function </summary>
  
  // <use visibility=export>
  // <prerequisite>
  //   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
  // <etymology>
  // WP for W-Projection 
  // ConvFunc => returns the convolution functions
  // </etymology>
  //
  // <synopsis> 
  // FTMachines like WProjection and MosaicFT need convolution functions to 
  // deal with directional dependent issues...
  // this class and related ones provide and cache  such functions for re-use 
  //</synopsis>

  class WPConvFunc 
    {
    public:
      WPConvFunc(const Double minW=-1.0, const Double maxW=-1.0, const Double rmsW=-1.0);
      WPConvFunc(const RecordInterface& rec);
      //Copy constructor
      WPConvFunc(const WPConvFunc& other);
      //
      WPConvFunc& operator=(const WPConvFunc&other);
      

      virtual ~WPConvFunc();

      // Inputs are the image, visbuffer,  wConvsize
      // findconv return a cached convolution function appropriate for this 
      // visbuffer and number of w conv plane
      void findConvFunction(const ImageInterface<Complex>& iimage, 
			    const vi::VisBuffer2& vb,
			    const Int& wConvSize,
			    const Vector<Double>& uvScale,
			    const Vector<Double>& uvOffset,
			    const Float& padding, 
			    Int& convSampling,
			    Cube<Complex>& convFunc, 
			    Int& convsize,
			    Vector<Int>& convSupport,
			    Double& wScale);

      Bool findSupport(Array<Complex>& /*func*/, Float& /*threshold*/,Int& /*origin*/, Int& /*R*/) 
    {throw(AipsError("IlluminationConvFunc::findSupport() not implemented"));};
      virtual Bool makeAverageResponse(const vi::VisBuffer2& /*vb*/,
				       const ImageInterface<Complex>& /*image*/,
				     //				     TempImage<Float>& theavgPB,
				       ImageInterface<Float>& /*theavgPB*/,
				       Bool /*reset=True*/)
    {throw(AipsError("WPConvFunc::makeAverageRes() called"));};
      //Serialization
      Bool toRecord(RecordInterface& rec);
      Bool fromRecord(String& err, const RecordInterface& rec);
    private:
      Bool checkCenterPix(const ImageInterface<Complex>& image);
      Block <CountedPtr<Cube<Complex> > > convFunctions_p;
      Block <CountedPtr<Vector<Int> > > convSupportBlock_p;
      SimpleOrderedMap <String, Int> convFunctionMap_p;
      Vector<Int> convSizes_p;

      Int actualConvIndex_p;
      Int convSize_p;
      Vector<Int> convSupport_p;
      Cube<Complex> convFunc_p;
      Double wScaler_p;
      Int convSampling_p;
      Int nx_p, ny_p;
      Double minW_p, maxW_p, rmsW_p;

    };
} //end of namespace refim
};// end of namespace casa
#endif