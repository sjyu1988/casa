// -*- mode: c++ -*-
//# ParallelImagerAdaptor.h: Adapt ParallelImagerMixin classes to ParallelImager
//#                          interface
//# Copyright (C) 2016
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
#ifndef PARALLEL_IMAGER_ADAPTOR_H_
#define PARALLEL_IMAGER_ADAPTOR_H_

#include <synthesis/ImagerObjects/ParallelImager.h>
#include <synthesis/ImagerObjects/ParallelImagerMixin.h>

namespace casa {

/**
 * Adaptor for ParallelImagerMixin implementation classes as ParallelImager
 * instances.
 */

template <class T>
class ParallelImagerAdaptor
	: public ParallelImager, public T
{
public:
	using T::T;

	virtual Record clean() {
		return T::clean();
	};
};

// Standard parallel imager classes based on implementation classes in
// ParallelImagerMixin.h

//Parallel continuum imager
typedef ParallelImagerAdaptor<ContinuumParallelImagerImpl> ContinuumParallelImager;

// Parallel cube imager
typedef ParallelImagerAdaptor<CubeParallelImagerImpl> CubeParallelImager;

// Serial imager
typedef ParallelImagerAdaptor<SerialParallelImagerImpl> SerialParallelImager;
} // namespace casa

#endif // PARALLEL_IMAGER_ADAPTOR_H_
