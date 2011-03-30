//# FFTServer.tcc: A class with methods for Fast Fourier Transforms
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2003
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
//# $Id: FFTServer.tcc 20932 2010-07-08 09:06:37Z gervandiepen $

#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Mathematics/NumericTraits.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/VectorIter.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicMath/Math.h>
#include <casa/Utilities/Assert.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class T, class S> FFTServer<T,S>::
  FFTServer()
  : itsTransformType (FFTEnums::REALTOCOMPLEX)
{}
  
template<class T, class S> FFTServer<T,S>::
FFTServer(const IPosition & fftSize, 
	  const FFTEnums::TransformType transformType)
  : itsTransformType (transformType)
{
  resize (fftSize, transformType);
}

template<class T, class S> FFTServer<T,S>::
FFTServer(const FFTServer<T,S> & other)
  : itsTransformType (other.itsTransformType)
{
  resize (other.itsSize, other.itsTransformType);
}


template<class T, class S> FFTServer<T,S>::
~FFTServer()
{
  for (uInt i=0; i<itsWork.nelements(); ++i) {
    delete itsWork[i];
  }
}

template<class T, class S> FFTServer<T,S> & FFTServer<T,S>::
operator=(const FFTServer<T,S> & other)
{
  if (this != &other) {
    for (uInt i=0; i<itsSize.nelements(); ++i) {
      delete itsWork[i];
      itsWork[i] = 0;
    }
    resize (other.itsSize, other.itsTransformType);
  }

  return *this;
}


template<class T, class S> void FFTServer<T,S>::
resize(const IPosition & fftSize,
       const FFTEnums::TransformType transformType)
{
  DebugAssert(fftSize.nelements() > 0, AipsError);
  DebugAssert(fftSize.product() > 0, AipsError);
  // Only resize if different type or size.
  uInt ndim = fftSize.nelements();
  if (transformType != itsTransformType  ||
      itsSize.nelements() != ndim  ||  fftSize != itsSize) {
    itsTransformType = transformType;
    itsSize.resize (ndim, False);  // to make assignment work!
    itsSize = fftSize;

#ifdef HAVE_FFTW3
    size_t nelem = itsSize.product();
    itsWorkIn.resize (nelem);
    itsWorkOut.resize (nelem / itsSize[0] * (itsSize[0]/2+1));
    itsWorkC2C.resize (nelem);
    IPosition transpose(ndim);
    for (uint i=0; i<ndim; ++i) {
      transpose[i] = itsSize[ndim-1-i];
    }
    switch (itsTransformType) {
    case FFTEnums::REALTOCOMPLEX:
      itsFFTW.plan_r2c(transpose, &(itsWorkIn[0]), &(itsWorkOut[0]));
      break;
    case FFTEnums::COMPLEXTOREAL:
      itsFFTW.plan_c2r(transpose, &(itsWorkOut[0]), &(itsWorkIn[0]));
      break;
    case FFTEnums::COMPLEX:
      itsFFTW.plan_c2c_forward(transpose, &(itsWorkC2C[0]));
      break;
    case FFTEnums::INVCOMPLEX:
      itsFFTW.plan_c2c_backward(transpose, &(itsWorkC2C[0]));
      break;
    case FFTEnums::REALSYMMETRIC:
      AlwaysAssert(itsTransformType != FFTEnums::REALSYMMETRIC, AipsError);
    }
 
#else
    // FFTPack uses a work array per dimension.
    // Delete unused work arrays.
    for (uInt i=ndim; i<itsWork.size(); ++i) {
      delete itsWork[i];
      itsWork[i] = 0;
    }
    // Create work arrays as needed.
    uInt oldSize = itsWork.size();
    itsWork.resize (ndim);
    for (uInt i=0; i<ndim; ++i) {
      if (i >= oldSize  ||  itsWork[i] == 0) {
        itsWork[i] = new Block<T>;
      }
    }
    // Now size the work arrays. There is one for each dimension.
    // Initialize the FFT.
    // Only along the first dimension a real <-> complex transform is done,
    // so it is treated separately.
    uInt fftLen = fftSize[0];
    uInt workSize = 0;
    uInt bufferLength = itsBuffer.nelements();
    switch (transformType) {
    case FFTEnums::COMPLEX:
    case FFTEnums::INVCOMPLEX:
      workSize = 4 * fftLen + 15;
      bufferLength = std::max(bufferLength, fftLen);
      break;
    case FFTEnums::REALTOCOMPLEX:
    case FFTEnums::COMPLEXTOREAL:
      workSize = 2 * fftLen + 15;
      break;
    case FFTEnums::REALSYMMETRIC:
      workSize = 3 * fftLen + 15;
      break;
    }
    itsWork[0]->resize (workSize); 
    switch (transformType) {
    case FFTEnums::COMPLEX:
    case FFTEnums::INVCOMPLEX:
      FFTPack::cffti(fftLen, itsWork[0]->storage());
      break;
    case FFTEnums::REALTOCOMPLEX:
    case FFTEnums::COMPLEXTOREAL:
      FFTPack::rffti(fftLen, itsWork[0]->storage());
      break;
    case FFTEnums::REALSYMMETRIC:
      FFTPack::costi(fftLen, itsWork[0]->storage());
      break;
    }
    // Allocate the work arrays for the other dimensions.
    for (uInt i=1; i<ndim; ++i) {
      fftLen = fftSize[i];
      workSize = 4 * fftLen + 15;
      itsWork[i]->resize (workSize); 
      FFTPack::cffti(fftLen, itsWork[i]->storage());
      bufferLength = std::max(bufferLength, fftLen);
    }
    // Resize the buffer.
    itsBuffer.resize (bufferLength, False, False);
#endif
  }
}

template<class T, class S> void FFTServer<T,S>::
fft(Array<S> & cResult, Array<T> & rData, const Bool constInput)
{
  if (constInput) {
    Array<T> rCopy = rData.copy();
    flip(rCopy,True,False);
    fft0(cResult, rCopy, False);
  } else {
    flip(rData,True,False);
    fft0(cResult, rData, False);
  }
  flip(cResult,False,True);
}

template<class T, class S> void FFTServer<T,S>::
fft(Array<S> & cResult, const Array<T> & rData)
{
 fft(cResult, (Array<T> &) rData, True);
}

template<class T, class S> void FFTServer<T,S>::
fft(Array<T> & rResult, Array<S> & cData, const Bool constInput)
{
  if (constInput) {
    Array<S> cCopy = cData.copy();
    flip(cCopy, True, True);
    fft0(rResult, cCopy, False);
  } else {
    flip(cData, True, True);
    fft0(rResult, cData, False);
  }
  flip(rResult, False, False);
}

template<class T, class S> void FFTServer<T,S>::
fft(Array<T> & rResult, const Array<S> & cData)
{
  fft(rResult, (Array<S> &) cData, True);
}

template<class T, class S> void FFTServer<T,S>::
fft(Array<S> & cValues, const Bool toFrequency)
{
  flip(cValues, True, False);
  fft0(cValues, toFrequency);
  flip(cValues, False, False);
}


template<class T, class S> void FFTServer<T,S>::
fft(Array<S> & cResult, const Array<S> & cData, const Bool toFrequency)
{
  if (cResult.nelements() != 0) {
    AlwaysAssert(cResult.conform(cData), AipsError);
  } else {
    cResult.resize(cData.shape());
  }
  cResult = cData;
  fft(cResult, toFrequency);
}

template<class T, class S> void FFTServer<T,S>::
fft0(Array<S> & cResult, Array<T> & rData, const Bool)
{
  const IPosition shape = rData.shape();
  // Ensure the output Array is the required size
  IPosition resultShape = shape;
  resultShape(0) = (shape(0)+2)/2;

  if (cResult.nelements() != 0) {
    AlwaysAssert(resultShape.isEqual(cResult.shape()), AipsError);
  } else {
    cResult.resize(resultShape);
  }
  // Early exit if the Array is all zero;
  if (allNearAbs(rData, T(0), NumericTraits<T>::minimum)) {
    cResult = S(0);
    return;
  }
  // Initialise the work arrays
  if (!shape.isEqual(itsSize) || itsTransformType != FFTEnums::REALTOCOMPLEX) {
    resize(shape, FFTEnums::REALTOCOMPLEX);
  }
  // get a pointer to the array holding the result
  Bool resultIsAcopy, dataIsAcopy;
  S * resultPtr = cResult.getStorage(resultIsAcopy);
  const T* dataPtr = rData.getStorage(dataIsAcopy);

#ifdef HAVE_FFTW3

  IPosition fftwShape(resultShape);
  objcopy(&(itsWorkIn[0]), dataPtr, itsWorkIn.size());
  itsFFTW.r2c(itsSize, &(itsWorkIn[0]), &(itsWorkOut[0]));
  objcopy(resultPtr, &(itsWorkOut[0]), itsWorkOut.size());

#else

  // Do real to complex transforms along all the rows
  {
    T * workPtr = itsWork[0]->storage();
    T * resPtr = (T *) resultPtr;
    uInt fftLen = shape(0);
    Bool even = True;
    if (fftLen%2 == 1) even = False;
    uInt resultRowLen = resultShape(0)*2;
    const T * inputRowPtr = dataPtr;
    T * resultRowPtr = resPtr;
    const uInt nrows = shape.product()/fftLen;
    // Iterate over all the rows
    for (uInt r = 0; r < nrows; ++r) {
      // Copy data to the complex array
      objcopy(resultRowPtr, inputRowPtr, fftLen);
      // Do the Real->Complex row transforms
      FFTPack::rfftf(fftLen, resultRowPtr, workPtr);
      // Shuffle elements along
      if (fftLen > 1) {
	objmove(resultRowPtr+2, resultRowPtr+1, fftLen-1);
      }
      // put zero into imaginary part of the first element
      *(resultRowPtr+1) = T(0.0);
      if (even) {
	// Stick zero into imaginary part of the nyquist sample
	*(resultRowPtr+resultRowLen-1) = T(0.0);
      } 
      // Increment the pointers
      inputRowPtr += fftLen;
      resultRowPtr += resultRowLen;
    }
  }
  // Do complex to complex transforms along all the remaining axes.
  const uInt ndim = shape.nelements();
  if (ndim > 1) {
    T * workPtr = 0;
    S * buffPtr = 0;
    S * rowPtr = 0;
    const uInt cElements = resultShape.product();
    uInt nffts, r, stride = resultShape(0);
    for (uInt n = 1; n < ndim; ++n) {
      uInt fftLen = resultShape(n);
      nffts = cElements/fftLen;
      workPtr = itsWork[n]->storage();
      buffPtr = itsBuffer.storage();
      rowPtr = resultPtr;
      r = 0;
      while (r < nffts) {
	// Copy the data into a temporary buffer. This makes it contiguous and
	// hence it is more likely to fit into cache. With current computers
	// this speeds up access to the data by a factors of about ten!
	objcopy(buffPtr, rowPtr, fftLen, 1u, stride);
	// Do the transform
	FFTPack::cfftf(fftLen, buffPtr, workPtr);
	// copy the data back
	objcopy(rowPtr, buffPtr, fftLen, stride, 1u);
	// indexing calculations
	r++;
	rowPtr++;
	if (r%stride == 0) {
	  rowPtr += stride*(fftLen-1);
        }
      }
      stride *= fftLen;
    }
  }

#endif
  rData.freeStorage(dataPtr, dataIsAcopy);
  cResult.putStorage(resultPtr, resultIsAcopy);
}
  
template<class T, class S> void FFTServer<T,S>::
fft0(Array<S> & cResult, const Array<T> & rData)
{
  fft0(cResult, (Array<T> &) rData, True);
}

template<class T, class S> void FFTServer<T,S>::
fft0(Array<T> & rResult, Array<S> & cData, const Bool constInput)
{
  Array<S> cCopy;
  if (constInput) {
    cCopy = cData;
  } else {
    cCopy.reference(cData);
  }
  const IPosition cShape = cCopy.shape();
  const IPosition rShape = determineShape(rResult.shape(), cCopy);
  rResult.resize(rShape);

  // Early exit if the Array is all zero;
  if (allNearAbs(cData, S(0), NumericTraits<S>::minimum)) {
    rResult = T(0);
    return;
  }
  // resize the server if necessary
  if (!rShape.isEqual(itsSize) || itsTransformType != FFTEnums::COMPLEXTOREAL) {
    resize(rShape, FFTEnums::COMPLEXTOREAL);
  }
  Bool dataIsAcopy, resultIsAcopy;
  S * dataPtr = cCopy.getStorage(dataIsAcopy);
  T *resultPtr = rResult.getStorage(resultIsAcopy);

#ifdef HAVE_FFTW3

  objcopy(&(itsWorkOut[0]), dataPtr, itsWorkOut.size());
  itsFFTW.c2r(itsSize, &(itsWorkOut[0]), &(itsWorkIn[0]));
  for (uInt i = 0; i < itsWorkIn.size(); i++) {
    itsWorkIn[i] /= 1.0*itsWorkIn.size();
  }
  objcopy(resultPtr, &(itsWorkIn[0]), itsWorkIn.size());

#else

  T * workPtr = 0;
  // Do complex to complex transforms along all other dimensions
  const uInt ndim = rShape.nelements();
  if (ndim > 1) {
    S * buffPtr = itsBuffer.storage();
    S * rowPtr = 0;
    const uInt cElements = cShape.product();
    uInt n, r, nffts, stride = cShape(0);
    for (n = 1; n < ndim; ++n) {
      workPtr = itsWork[n]->storage();
      rowPtr = dataPtr;
      uInt fftLen = rShape(n);
      nffts = cElements/fftLen;
      r = 0;
      while (r < nffts) {
	// Copy the data into a temporary buffer. This makes it contiguous and
	// hence it is more likely to fit into cache. With current computers
	// this speeds up access to the data by a factors of about ten!
	objcopy(buffPtr, rowPtr, fftLen, 1u, stride);
	// Do the FFT
	FFTPack::cfftb(fftLen, buffPtr, workPtr);
	// copy the data back
	objcopy(rowPtr, buffPtr, fftLen, stride, 1u);
	// indexing calculations
	r++;
	rowPtr++;
	if (r%stride == 0) {
	  rowPtr += stride*(fftLen-1);
        }
      }
      stride *= fftLen;
    }
  }

  T * realDataPtr = (T *) dataPtr;
  workPtr = itsWork[0]->storage();

  T * resultRowPtr = resultPtr;
  const uInt cStride = cShape(0)*2;
  uInt fftLen = rShape(0);
  const uInt nffts = rShape.product()/fftLen;
  // Iterate over all the rows
  for (uInt r = 0; r < nffts; ++r) {
    // Copy the data to the real array
    *resultRowPtr = *realDataPtr;
    objcopy(resultRowPtr+1, realDataPtr+2, fftLen-1);
    // Do the Complex->Real row transform
      FFTPack::rfftb(fftLen, resultRowPtr, workPtr);
    // Increment the pointers
    realDataPtr += cStride;
    resultRowPtr += fftLen;
  }
  // While we have a raw pointer handy do the scaling
  uInt nelem = rResult.nelements();
  T scale = T(1)/T(nelem);
  T * endPtr = resultPtr + nelem;
  for (resultRowPtr = resultPtr; resultRowPtr < endPtr; resultRowPtr++) {
    *resultRowPtr *= scale;
  }

#endif
  rResult.putStorage(resultPtr, resultIsAcopy);
  cCopy.freeStorage((const S*&)dataPtr, dataIsAcopy);
}

template<class T, class S> void FFTServer<T,S>::
fft0(Array<T> & rResult, const Array<S> & cData)
{
  fft0(rResult, (Array<S> &) cData, True);
}


template<class T, class S> void FFTServer<T,S>::
fft0(Array<S> & cValues, const Bool toFrequency)
{
  // Early exit if the Array is all zero;
  if (allNearAbs(cValues, S(0), NumericTraits<S>::minimum)) {
    return;
  }
  // resize the server if necessary
  const IPosition shape = cValues.shape();
  if (toFrequency) {
    if (!shape.isEqual(itsSize) || itsTransformType != FFTEnums::COMPLEX) {
      resize(shape, FFTEnums::COMPLEX);
    }
  } else {
    if (!shape.isEqual(itsSize) || itsTransformType != FFTEnums::INVCOMPLEX) {
      resize(shape, FFTEnums::INVCOMPLEX);
    }
  }
  Bool valuesIsAcopy;
  S * complexPtr = cValues.getStorage(valuesIsAcopy);

#ifdef HAVE_FFTW3

  objcopy(&(itsWorkC2C[0]), complexPtr, itsWorkC2C.size());
  itsFFTW.c2c(itsSize, &(itsWorkC2C[0]), toFrequency);
  if (!toFrequency) {
    for (uInt i = 0; i < itsWorkC2C.size(); ++i) {
      itsWorkC2C[i] /= 1.0*itsWorkC2C.size();
    }
  }
  objcopy(complexPtr, &(itsWorkC2C[0]), itsWorkC2C.size());

#else

  const uInt ndim = shape.nelements();
  T * workPtr = 0;
  // Do complex to complex transforms along all the dimensions
  S * buffPtr = itsBuffer.storage();
  T * realBuffPtr = 0;
  T * endRowPtr = 0;
  S * rowPtr = 0;
  const uInt nElements = shape.product();
  const T scale = T(1)/T(nElements);
  const uInt shape0t2 = shape(0) * 2;
  uInt n, r, nffts, stride = 1u;
  for (n = 0; n < ndim; ++n) {
    workPtr = itsWork[n]->storage();
    rowPtr = complexPtr;
    uInt fftLen = shape(n);
    nffts = nElements/fftLen;
    r = 0;
    buffPtr = itsBuffer.storage();
    while (r < nffts) {
      // Copy the data into a temporary buffer. This makes it contiguous and
      // hence it is more likely to fit into cache. With current computers
      // this speeds up access to the data by a factors of about ten!
      if (n != 0) {
	objcopy(buffPtr, rowPtr, fftLen, 1u, stride);
      } else {
	buffPtr = rowPtr;
      }
      // Do the FFT
      if (toFrequency == True) {
	FFTPack::cfftf(fftLen, buffPtr, workPtr);
      } else {
	FFTPack::cfftb(fftLen, buffPtr, workPtr);
	if (n == 0) {  // Scale by 1/N while things are (hopefully) in cache
	  realBuffPtr = (T *) buffPtr;
	  // No need to do complex multiplications when real ones will do. 
	  // This saves two multiplies and additions per complex element.
	  for (endRowPtr = realBuffPtr+shape0t2; 
	       realBuffPtr < endRowPtr; realBuffPtr++) {
	    *realBuffPtr *= scale;
	  }
	}
      }
      // copy the data back
      if (n != 0) {
	objcopy(rowPtr, buffPtr, fftLen, stride, 1u);
      }
      // indexing calculations
      r++;
      rowPtr++;
      if (r%stride == 0) {
	rowPtr += stride*(fftLen-1);
      }
    }
    stride *= fftLen;
  }

#endif
  cValues.putStorage(complexPtr, valuesIsAcopy);
}

  objcopy(complexPtr, itsWorkC2C, itsSize.product());
  cValues.putStorage(complexPtr, valuesIsAcopy);

  return;

}


template<class T, class S> void FFTServer<T,S>::
fft0(Array<S> & cResult, const Array<S> & cData, const Bool toFrequency)
{
  if (cResult.nelements() != 0) {
    AlwaysAssert(cResult.conform(cData), AipsError);
  } else {
    cResult.resize(cData.shape());
  }
  cResult = cData;
  fft0(cResult, toFrequency);
}


template<class T, class S> IPosition FFTServer<T,S>::
determineShape(const IPosition & rShape, const Array<S> & cData)
{
  const IPosition cShape=cData.shape();
  const uInt cDim = cShape.nelements();
  DebugAssert(cDim > 0, AipsError);
  // If rShape is non-zero then it must match one of the two possible shapes
  if (rShape.product() != 0) {
    DebugAssert(cDim == rShape.nelements(), AipsError);
    IPosition reqShape(cShape);
    reqShape(0) = 2*cShape(0)-2;
    if (reqShape.isEqual(rShape)) {
      return reqShape;
    }
    reqShape(0) += 1;
    if (reqShape.isEqual(rShape)) {
      return reqShape;
    }
    throw(AipsError("FFTServer<T,S>::determineShape() -"
		    " output array has the wrong shape"));
  }
  // Scan the imaginary components of the last samples on the first axis in
  // the cData to see if there are any non-zero terms. If so the output array
  // must be odd length in its first axis.
  {
    VectorIterator<S> iter((Array<S> &) cData);
    uInt lastElem = cShape(0)-1;
    while (!iter.pastEnd()) {
      if (!near(iter.vector()(lastElem).imag(), (T)0.0)) {
	IPosition oddLength(cShape);
	oddLength(0) = cShape(0)*2-1;
	return oddLength;
      }
      iter.next();
    }
  }
  // See if the FFTServer size can be used to guess the output Array size;
  if (itsSize.nelements() == cDim) {
    Bool match = True;
    for (uInt i = 1; i < cDim; ++i) {
      if (itsSize(i) != cShape(i)) {
	match = False;
      }
    }
    if (match == True && 
	((itsSize(0) == 2*cShape(0) - 2) || (itsSize(0) == 2*cShape(0) - 1))) {
      return itsSize;
    }
  }
  IPosition defShape(cShape);
  defShape(0) = 2*cShape(0) - 2;

  return defShape;
}

template<class T, class S> void FFTServer<T,S>::
flip(Array<S> & cData, const Bool toZero, const Bool isHermitian)
{
  const IPosition shape = cData.shape();
  const uInt ndim = shape.nelements();
  const uInt nElements = cData.nelements();
  if (nElements == 1) {
    return;
  }
  AlwaysAssert(nElements != 0, AipsError);
  {
    Int buffLen = itsBuffer.nelements();
    for (uInt i = 0; i < ndim; ++i) {
      buffLen = max(buffLen, shape(i));
    }
    itsBuffer.resize(buffLen, False, False);
  }
  Bool dataIsAcopy;
  S * dataPtr = cData.getStorage(dataIsAcopy);
  S * buffPtr = itsBuffer.storage();
  S * rowPtr = 0;
  S * rowPtr2 = 0;
  S * rowPtr2o = 0;
  uInt rowLen, rowLen2, rowLen2o;
  uInt nFlips;
  uInt stride = 1;
  uInt r;
  uInt n=0;
  if (isHermitian) {
    n = 1;
    stride = shape(0);
  }
  for (; n < ndim; ++n) {
    rowLen = shape(n);
    if (rowLen > 1) {
      rowLen2 = rowLen/2;
      rowLen2o = (rowLen+1)/2;
      nFlips = nElements/rowLen;
      rowPtr = dataPtr;
      r = 0;
      while (r < nFlips) {
	rowPtr2 = rowPtr + stride * rowLen2;
	rowPtr2o = rowPtr + stride * rowLen2o;
	if (toZero) {
	  objcopy(buffPtr, rowPtr2, rowLen2o, 1u, stride);
	  objcopy(rowPtr2o, rowPtr, rowLen2, stride, stride);
	  objcopy(rowPtr, buffPtr, rowLen2o, stride, 1u);
	} else {
	  objcopy(buffPtr, rowPtr, rowLen2o, 1u, stride);
	  objcopy(rowPtr, rowPtr2o, rowLen2, stride, stride);
	  objcopy(rowPtr2, buffPtr, rowLen2o, stride, 1u);
	}
	r++;
	rowPtr++;
	if (r%stride == 0) {
	  rowPtr += stride*(rowLen-1);
        }
      }
      stride *= rowLen;
    }
  }
  cData.putStorage(dataPtr, dataIsAcopy);
}

template<class T, class S> void FFTServer<T,S>::
flip(Array<T> & rData, const Bool toZero, const Bool isHermitian)
{
  const IPosition shape = rData.shape();
  const uInt ndim = shape.nelements();
  const uInt nElements = rData.nelements();
  if (nElements == 1) {
    return;
  }
  AlwaysAssert(nElements != 0, AipsError);
  {
    Int buffLen = itsBuffer.nelements();
    for (uInt i = 0; i < ndim; ++i) {
      buffLen = max(buffLen, (shape(i)+1)/2);
    }
    itsBuffer.resize(buffLen, False, False);
  }
  Bool dataIsAcopy;
  T * dataPtr = rData.getStorage(dataIsAcopy);
  T * buffPtr = (T *) itsBuffer.storage();
  T * rowPtr = 0;
  T * rowPtr2 = 0;
  T * rowPtr2o = 0;
  uInt rowLen, rowLen2, rowLen2o;
  uInt nFlips;
  uInt stride = 1;
  uInt r;
  uInt n=0;
  if (isHermitian) {
    n = 1;
    stride = shape(0);
  }
  for (; n < ndim; ++n) {
    rowLen = shape(n);
    if (rowLen > 1) {
      rowLen2 = rowLen/2;
      rowLen2o = (rowLen+1)/2;
      nFlips = nElements/rowLen;
      rowPtr = dataPtr;
      r = 0;
      while (r < nFlips) {
	rowPtr2 = rowPtr + stride * rowLen2;
	rowPtr2o = rowPtr + stride * rowLen2o;
	if (toZero) {
	  objcopy(buffPtr, rowPtr2, rowLen2o, 1u, stride);
	  objcopy(rowPtr2o, rowPtr, rowLen2, stride, stride);
	  objcopy(rowPtr, buffPtr, rowLen2o, stride, 1u);
	} else {
	  objcopy(buffPtr, rowPtr, rowLen2o, 1u, stride);
	  objcopy(rowPtr, rowPtr2o, rowLen2, stride, stride);
	  objcopy(rowPtr2, buffPtr, rowLen2o, stride, 1u);
	}
	r++;
	rowPtr++;
	if (r%stride == 0) {
	  rowPtr += stride*(rowLen-1);
        }
      }
      stride *= rowLen;
    }
  }
  rData.putStorage(dataPtr, dataIsAcopy);
}

template<class T, class S> void FFTServer<T,S>::
fftshift(Array<S> & cValues, const uInt& whichAxis,
	 const Double& relshift, const Bool toFrequency)
{
  const IPosition arrayShape = cValues.shape();
  const uInt vsize = arrayShape[whichAxis];
  AlwaysAssert(vsize > 0, AipsError);
      
  // relshift is the freq shift normalised to the bandwidth
  const Complex exponent =  2.*C::pi*Complex(0.,1.)*relshift; 
      
  ArrayIterator<S> ait(cValues, IPosition(1,whichAxis), True); // axes are the cursor  
  while(!ait.pastEnd()){
    Array<S> cv = ait.array(); // reference
    fft0(cv, toFrequency);
    for(uInt i=0; i<vsize; i++){
      cv(IPosition(1,i)) *= exp(Double(i)*exponent);
    }
    fft0(cv, !toFrequency);
    ait.next();
  }
}

template<class T, class S> void FFTServer<T,S>::
fftshift(Array<S> & outValues, Array<Bool> & outFlags,
	 const Array<S> & cValues, const Array<Bool> & inFlags,
	 const uInt& whichAxis, 
	 const Double& relshift, 
	 const Bool goodIsTrue,
	 const Bool toFrequency){

  const IPosition arrayShape = cValues.shape();
  const Int vsize = arrayShape[whichAxis];
  const IPosition fArrayShape = inFlags.shape();
  AlwaysAssert(vsize > 0, AipsError);
  AlwaysAssert(arrayShape==fArrayShape, AipsError);
  AlwaysAssert(abs(relshift)<1.,AipsError);
  
  outValues.assign(cValues);
  outFlags.assign(inFlags);
    
  // relshift is the freq shift normalised to the bandwidth
  const Complex exponent =  2.*C::pi*Complex(0.,1.)*relshift; 

  Int numToFlag = ceil(vsize*abs(relshift));
      
  ArrayIterator<S> ait(outValues, IPosition(1,whichAxis), True); // axes are the cursor  
  ArrayIterator<Bool> fait(outFlags, IPosition(1,whichAxis), True); // axes are the cursor  
  while(!ait.pastEnd()){
    Array<S> cv = ait.array(); // reference
    Array<Bool> flags = fait.array(); // reference

    // set flagged channels to zero
    for(Int i=0; i<vsize; i++){
      if(flags(IPosition(1,i))!=goodIsTrue){ // this channel is flagged
	cv(IPosition(1,i)) = (S)0.; // set to zero if flagged
      }
    }

    // apply shift
    fft0(cv, toFrequency);
    for(Int i=0; i<vsize; i++){
      cv(IPosition(1,i)) *= exp(Double(i)*exponent);
    }
    fft0(cv, !toFrequency);

    // generate the new flags
    if(relshift>0.){
      for(Int i=vsize-1-numToFlag; i>=0; i--){
	if( (flags(IPosition(1,i))!=goodIsTrue) && (i+numToFlag < vsize)){ // this channel is flagged
	  flags(IPosition(1,i+numToFlag)) = !goodIsTrue;
	  flags(IPosition(1,i)) = goodIsTrue;
	}
      }
    }
    else{
      for(Int i=numToFlag; i<vsize; i++){
	if( (flags(IPosition(1,i))!=goodIsTrue) && (0 <= i-numToFlag) ){ // this channel is flagged
	  flags(IPosition(1,i-numToFlag)) = !goodIsTrue;
	  flags(IPosition(1,i)) = goodIsTrue;
	}
      }
    }      
    // flag the edge channels which were wrapped by the shift
    if(relshift>0.){ // start at bottom
      for(Int i=0; i<numToFlag; i++){
	flags(IPosition(1,i)) = !goodIsTrue;
      }
    }
    else{ // start at top
      for(Int i=vsize-1; i>vsize-1-numToFlag; i--){
	flags(IPosition(1,i)) = !goodIsTrue;
      }
    }      

    ait.next();
    fait.next();
  } 

}

} //# NAMESPACE CASA - END
