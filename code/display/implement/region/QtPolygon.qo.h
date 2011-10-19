//# QtPolygon.h: base class for statistical regions
//# Copyright (C) 2011
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


#ifndef REGION_QTPOLYGON_H_
#define REGION_QTPOLYGON_H_

#include <display/region/QtRegion.qo.h>
#include <display/region/Polygon.h>

namespace casa {
    namespace viewer {

	class QtRegionSource;

	// All regions are specified in "linear coordinates", not "pixel coordinates". This is necessary
	// because "linear coordinates" scale with zooming whereas "pixel coordinates" do not. Unfortunately,
	// this means that coordinate transformation is required each time the region is drawn.
	//
	// Key points:
	//    <ul>
	//        <li> regions are produced by a factory to permit the creation of gui specific regions </li>
	//    </ul>
	class QtPolygon : public QtRegion, public Polygon {

	    public:

		const std::string name( ) const { return QtRegion::name( ); }

		std::string lineColor( ) const { return QtRegion::lineColor( ); }
		int lineWidth( ) const { return QtRegion::lineWidth( ); }
		Region::LineStyle lineStyle( ) const { return QtRegion::lineStyle( ); }

		std::string textColor( ) const { return QtRegion::textColor( ); }
		std::string textFont( ) const { return QtRegion::textFont( ); }
		int textFontSize( ) const { return QtRegion::textFontSize( ); }
		int textFontStyle( ) const { return QtRegion::textFontStyle( ); }
		std::string textValue( ) const { return QtRegion::textValue( ); }
		Region::TextPosition textPosition( ) const { return QtRegion::textPosition( ); }
		void textPositionDelta( int &x, int &y ) const { QtRegion::textPositionDelta( x, y ); }

		void getCoordinatesAndUnits( Region::Coord &c, Region::Units &u ) const
			{ Region::getCoordinatesAndUnits( c, u ); }
		void getPositionString( std::string &x, std::string &y, std::string &angle,
					Region::Coord coord = Region::DefaultCoord,
					Region::Units units = Region::DefaultUnits ) const
			{ Region::getPositionString( x, y, angle, coord, units ); }
		void movePosition( const std::string &x, const std::string &y,
				   const std::string &coord, const std::string &units )
			{ Region::movePosition( x, y, coord, units ); }

		int numFrames( ) const { return QtRegion::numFrames( ); }
		void zRange( int &min, int &max ) const { QtRegion::zRange(min,max); }
		int zIndex( ) const { return Region::zIndex( ); }

		// indicates that region movement requires that the statistcs be updated...
		void updateStateInfo( bool region_modified ) { QtRegion::updateStateInfo( region_modified ); }

		void clearStatistics( ) { QtRegion::clearStatistics( ); }

		QtPolygon( QtRegionSource *factory, WorldCanvas *wc, double x1, double y1, bool hold_signals=false );
		QtPolygon( QtRegionSource *factory, WorldCanvas *wc, const std::vector<std::pair<double,double> > &pts, bool hold_signals=false );

		bool regionVisible( ) const { return Region::regionVisible( ); }
		void regionCenter( double &x, double &y ) const { Polygon::regionCenter( x, y ); }

		// qt-event -> QtRegion -> QtPolygon -> Region::refresh( )
		void refresh( ) { Polygon::refresh( ); }
		AnnotationBase *annotation( ) const { return Polygon::annotation( ); }

		// indicates that the user has selected this rectangle...
		void selectedInCanvas( ) { QtRegion::selectedInCanvas( ); }

		void setLabel( const std::string &l ) { QtRegion::setLabel(l); }
		void setFont( const std::string &font="", int font_size=-1, int font_style=0, const std::string &font_color="" )
				{ QtRegion::setFont( font, font_size, font_style, font_color ); }
		void setLine( const std::string &line_color="", Region::LineStyle line_style=SolidLine )
				{ QtRegion::setLine( line_color, line_style ); }

	    protected:
		std::list<RegionInfo> *generate_dds_statistics( ) { return Polygon::generate_dds_statistics( ); }
		virtual Region *fetch_my_region( ) { return (Region*) this; }
		virtual void fetch_region_details( RegionTypes &type, std::vector<std::pair<int,int> > &pixel_pts, 
						   std::vector<std::pair<double,double> > &world_pts ) const 
				{ return Polygon::fetch_region_details( type, pixel_pts, world_pts ); }

	};
    }
}

#endif
