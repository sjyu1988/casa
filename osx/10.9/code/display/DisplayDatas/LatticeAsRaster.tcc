//# LatticeAsRaster.cc: Class to display lattice objects as rastered images
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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
//# $Id$

#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Containers/Record.h>
#include <lattices/Lattices/Lattice.h>
#include <images/Images/ImageInterface.h>
#include <display/DisplayDatas/LatticePADMRaster.h>
#include <display/Display/Attribute.h>
#include <display/DisplayCanvas/WCPowerScaleHandler.h>
#include <display/DisplayDatas/LatticeAsRaster.h>


namespace casa { //# NAMESPACE CASA - BEGIN

	template <class T>
	const String LatticeAsRaster<T>::HISTOGRAM_RANGE = "minmaxhist";
	template <class T>
	const String LatticeAsRaster<T>::COLOR_MODE = "colormode";


// >2d array-based ctor
	template <class T>
	LatticeAsRaster<T>::LatticeAsRaster(Array<T> *array, const uInt xAxis,
	                                    const uInt yAxis, const uInt mAxis,
	                                    const IPosition fixedPos) :
		LatticePADisplayData<T>(array, xAxis, yAxis, mAxis, fixedPos) {
		setupElements();
		String attString("colormodel");
		Attribute attColor(attString, Int(Display::Index));
		setAttribute(attColor);
		itsPowerScaleHandler = new WCPowerScaleHandler;
		setDefaultOptions();
	}

// 2d array-based ctor
	template <class T>
	LatticeAsRaster<T>::LatticeAsRaster(Array<T> *array, const uInt xAxis,
	                                    const uInt yAxis) :
		LatticePADisplayData<T>(array, xAxis, yAxis) {
		setupElements();
		String attString("colormodel");
		Attribute attColor(attString, Int(Display::Index));
		setAttribute(attColor);
		itsPowerScaleHandler = new WCPowerScaleHandler;
		setDefaultOptions();
	}

// >2d image-based ctor
	template <class T>
	LatticeAsRaster<T>::LatticeAsRaster( shared_ptr<ImageInterface<T> > image,const uInt xAxis, const uInt yAxis, const uInt mAxis, const IPosition fixedPos, viewer::StatusSink *sink ) :
		LatticePADisplayData<T>( image, xAxis, yAxis, mAxis, fixedPos, sink ) {
		setupElements();
		String attString("colormodel");
		Attribute attColor(attString, Int(Display::Index));
		setAttribute(attColor);
		itsPowerScaleHandler = new WCPowerScaleHandler;
		setDefaultOptions();
	}

// 2d image-based ctor
	template <class T>
	LatticeAsRaster<T>::LatticeAsRaster(shared_ptr<ImageInterface<T> > image,
	                                    const uInt xAxis, const uInt yAxis) :
		LatticePADisplayData<T>(image, xAxis, yAxis) {
		setupElements();
		String attString("colormodel");
		Attribute attColor(attString, Int(Display::Index));
		setAttribute(attColor);
		itsPowerScaleHandler = new WCPowerScaleHandler;
		setDefaultOptions();
	}

	template <class T>
	LatticeAsRaster<T>::~LatticeAsRaster() {
		for (uInt i=0; i<nelements(); i++) if(DDelement[i]!=0)
				delete static_cast<LatticePADMRaster<T>*>(DDelement[i]);
		if (itsPowerScaleHandler) {
			delete itsPowerScaleHandler;
		}
	}

// Ok, here we setup the elements using LatticePADMRaster
	template <class T>
	void LatticeAsRaster<T>::setupElements() {

		for (uInt i=0; i<nelements(); i++) if(DDelement[i]!=0) {
				delete static_cast<LatticePADMRaster<T>*>(DDelement[i]);
				DDelement[i]=0;
			}
		// Delete old DMs, if any.

		IPosition fixedPos = fixedPosition();
		Vector<Int> dispAxes = displayAxes();
		if (nPixelAxes > 2) {
			setNumImages(dataShape()(dispAxes(2)));
			DDelement.resize(nelements());
			for (uInt index = 0; index < nelements(); index++) {
				fixedPos(dispAxes(2)) = index;
				DDelement[index] = (LatticePADisplayMethod<T> *)new
				                   LatticePADMRaster<T>(dispAxes(0), dispAxes(1), dispAxes(2),
				                                        fixedPos, this);
			}
		} else {
			setNumImages(1);
			DDelement.resize(nelements());
			DDelement[0] = (LatticePADisplayMethod<T> *)new
			               LatticePADMRaster<T>(dispAxes(0), dispAxes(1), this);
		}

		itsOptionsDataRange.resize(2);
		itsOptionsDataDefault.resize(2);

		rasterRed = NULL;
		rasterBlue = NULL;
		rasterGreen = NULL;
		PrincipalAxesDD::setupElements();
	}

	template <class T>
	LatticeAsRaster<T>* LatticeAsRaster<T>::getRasterRed(){
		return rasterRed;
	}

	template <class T>
	LatticeAsRaster<T>* LatticeAsRaster<T>::getRasterGreen(){
		return rasterGreen;
	}

	template <class T>
	LatticeAsRaster<T>* LatticeAsRaster<T>::getRasterBlue(){
		return rasterBlue;
	}

	template <class T>
	void LatticeAsRaster<T>::setDisplayDataRed( DisplayData* dd ){
		if ( typeid(*dd) == typeid(*this)){
			rasterRed = dynamic_cast<LatticeAsRaster<T>* >(dd);
		}
	}

	template <class T>
	void LatticeAsRaster<T>::setDisplayDataBlue( DisplayData* dd ){
		if ( typeid(*dd) == typeid(*this)){
			rasterBlue = dynamic_cast<LatticeAsRaster<T>* >(dd);
		}
	}

	template <class T>
	void LatticeAsRaster<T>::setDisplayDataGreen( DisplayData* dd ){
		if ( typeid(*dd) == typeid(*this)){
			rasterGreen = dynamic_cast<LatticeAsRaster<T>* >(dd);
		}
	}

	template <class T>
	void LatticeAsRaster<T>::initializeDataMatrix( int index,
			Matrix<T>& datMatrix, Matrix<Bool>& mask, const IPosition& start,
			const IPosition& sliceShape, const IPosition& stride ){
		if ( index < static_cast<int>(DDelement.size())){
			((LatticePADisplayMethod<T> *)(DDelement[index]))->dataGetSlice( datMatrix, mask, start, sliceShape, stride);
		}
		else {
			cerr<<"Cannot compute data slice for index="<<index<<" DDelement size="<<DDelement.size();
		}
	}

	template <class T>
	void LatticeAsRaster<T>::setDefaultOptions() {

		LatticePADisplayData<T>::setDefaultOptions();

		itsOptionsDataRange(0) = getDataMin();
		itsOptionsDataRange(1) = getDataMax();
		itsOptionsDataDefault(0) = getDataMin();
		itsOptionsDataDefault(1) = getDataMax();

		itsOptionsColorMode = "colormap";
		//itsOptionsPower = 0.0;
		//itsPowerScaleHandler = new WCPowerScaleHandler;
		//itsPowerScaleHandler->setCycles(itsOptionsPower);
	}

	template <class T>
	Bool LatticeAsRaster<T>::setOptions(Record &rec, Record &recOut) {

		Bool localchange = False;
		Bool error;

		//Do this to save some headaches later. Ensure the value array of minmaxhist
		//(if it exists) is a float.

		if (rec.isDefined(HISTOGRAM_RANGE)) {
			if (rec.dataType(HISTOGRAM_RANGE) == TpRecord) {
				Record minmax = rec.subRecord(HISTOGRAM_RANGE);
				if (minmax.isDefined("value")) {
					DataType theType = minmax.dataType("value");

					if (theType != TpArrayFloat) {
						Vector<Float> newValue(minmax.toArrayFloat("value"));
						minmax.removeField("value");
						minmax.define("value", newValue);
						rec.defineRecord(HISTOGRAM_RANGE, minmax);
					}
				}
			} else {
				if (rec.dataType(HISTOGRAM_RANGE) != TpArrayFloat) {
					Vector<Float> newValue(rec.toArrayFloat(HISTOGRAM_RANGE));
					rec.removeField(HISTOGRAM_RANGE);
					rec.define(HISTOGRAM_RANGE, newValue);
				}
			}
		}



		Bool ret = LatticePADisplayData<T>::setOptions(rec, recOut);

		//Check for a change
		localchange = ((readOptionRecord(itsOptionsDataRange, error, rec, HISTOGRAM_RANGE)) ||
		               localchange );
		localchange = (itsPowerScaleHandler->setOptions(rec, recOut) ||
		               localchange);
		localchange = (readOptionRecord(itsOptionsColorMode, error, rec, COLOR_MODE) ||
		               localchange);


		ret = (ret || localchange);
		return ret;
	}




	template <class T>
	Record LatticeAsRaster<T>::getOptions( bool scrub ) const {

		Record rec = LatticePADisplayData<T>::getOptions(scrub);

		((LatticeAsRaster<T>*)this)->itsOptionsDataDefault(0) = Float(getDataMin());
		((LatticeAsRaster<T>*)this)->itsOptionsDataDefault(1) = Float(getDataMax());

		Record minmaxhist;
		minmaxhist.define("dlformat", HISTOGRAM_RANGE);
		minmaxhist.define("listname", "Data Range");
		minmaxhist.define("ptype", HISTOGRAM_RANGE);
		minmaxhist.define("pmin", Float(getDataMin()));
		minmaxhist.define("pmax", Float(getDataMax()));
		minmaxhist.define("default", itsOptionsDataDefault);
		minmaxhist.define("value", itsOptionsDataRange);
		minmaxhist.defineRecord("histarray", getHist());
		minmaxhist.define("imageunits", getBrightnessUnits());
		minmaxhist.define("allowunset", False);
		rec.defineRecord(HISTOGRAM_RANGE, minmaxhist);

		Record colormode;
		colormode.define("dlformat", "colormode");
		colormode.define("listname", "Color mode");
		colormode.define("ptype", "choice");
		Vector<String> vcolormode(7);
		vcolormode(0) = "colormap";
		vcolormode(1) = "red";
		vcolormode(2) = "green";
		vcolormode(3) = "blue";
		vcolormode(4) = "hue";
		vcolormode(5) = "saturation";
		vcolormode(6) = "value";
		colormode.define("popt", vcolormode);
		colormode.define("default", vcolormode(0));
		colormode.define("value", itsOptionsColorMode);
		colormode.define("allowunset", False);
		rec.defineRecord(COLOR_MODE, colormode);

		Record powerscalerec = itsPowerScaleHandler->getOptions();
		rec.merge(powerscalerec);

		return rec;
	}










} //# NAMESPACE CASA - END

