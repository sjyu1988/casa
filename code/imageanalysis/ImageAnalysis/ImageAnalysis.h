//# ImageAnalysis.h: Image analysis and handling tool
//# Copyright (C) 2007
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#ifndef _IMAGEANALYSIS__H__
#define _IMAGEANALYSIS__H__

// PLEASE DO *NOT* ADD ADDITIONAL METHODS TO THIS CLASS

#include <casa/Quanta/Quantum.h>

#include <imageanalysis/ImageTypedefs.h>

#include <utility>

namespace casacore{

class CoordinateSystem;
class ImageRegion;
class LatticeExprNode;
class LELImageCoord;
class RecordInterface;
}

namespace casa {

class ImageMomentsProgressMonitor;

//template<class T> class ImageHistograms;

// <summary>
// Image analysis and handling tool
// </summary>

// <synopsis>
// This the casapy image tool.
// One time it should be merged with pyrap's image tool ImageProxy.
// </synopsis>

// NOTE: NEW METHODS SHOULD NOT BE ADDED TO THIS CLASS. PLEASE USE THE ImageTask.h
// architecture for adding new functionality for image analysis. If you do not understand,
// please consult with me, dmehring@nrao.edu. If you add new methods to ImageAnalysis, I will contact
// you to remove them. Please save us both the annoyance of that.

class ImageAnalysis {
public:

    ImageAnalysis();

    ImageAnalysis(SPIIF image);

    ImageAnalysis(SPIIC image);

    virtual ~ImageAnalysis();

    /*
    inline static casacore::String className() {const static casacore::String x = "ImageAnalysis"; return x; }

    // get the associated casacore::ImageInterface object
    */
    SPCIIF getImage() const;
    /*
    SPIIF getImage();

    SPCIIC getComplexImage() const;

    SPIIC getComplexImage();

    casacore::Bool isFloat() const { return _imageFloat ? true : false; }
*/
 private:
    SPIIF _imageFloat;
    SPIIC _imageComplex;

    std::unique_ptr<casacore::LogIO> _log;

    casacore::IPosition last_chunk_shape_p;
   
    void _onlyFloat(const casacore::String& method) const;

    template<class T> static void _destruct(casacore::ImageInterface<T>& image);

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageAnalysis2.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
