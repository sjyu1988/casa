//# MultiPointTool.cc: Base class for MultiWorldCanvas event-based point tools
//# Copyright (C) 2000,2001,2002
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

#include <display/DisplayEvents/MultiPointTool.h>

namespace casa { //# NAMESPACE CASA - BEGIN

    std::tr1::shared_ptr<viewer::Rectangle> MultiPointTool::allocate_region( WorldCanvas *wc, double x, double y, double, double ) const {
	return rfactory->point( wc, x, y, rfactory->currentPointSymbolType( ) );
    }

    static std::set<viewer::Region::RegionTypes> multi_point_tool_region_set;
    const std::set<viewer::Region::RegionTypes> &MultiPointTool::regionsCreated( ) const {
	if ( multi_point_tool_region_set.size( ) == 0 ) {
	    multi_point_tool_region_set.insert( viewer::Region::PointRegion );
	}
	return multi_point_tool_region_set;
    }


} //# NAMESPACE CASA - END
