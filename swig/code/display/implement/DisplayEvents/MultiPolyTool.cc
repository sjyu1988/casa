//# MultiPolyTool.cc: Base class for MulitWorldCanvas event-based polygon tools
//# Copyright (C) 1999,2000,2001,2002
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
#include <casa/Arrays/ArrayMath.h>
#include <display/Display/WorldCanvas.h>
#include <display/Display/PixelCanvas.h>
#include <display/region/Polygon.h>
#include <display/DisplayEvents/MultiPolyTool.h>
#include <casadbus/types/nullptr.h>

namespace casa { //# NAMESPACE CASA - BEGIN

    MultiPolyTool::MultiPolyTool( viewer::RegionSourceFactory *rcs, PanelDisplay* pd,
			      Display::KeySym keysym, const Bool persistent ) :
		RegionTool(keysym),itsPolygonPersistent(persistent), itsMode(Off),
		itsEmitted(False), itsNPoints(0), itsHandleSize(7),
		rfactory(rcs->newSource(this)), pd_(pd) {
	reset();
	itsX.resize(1024);
	itsY.resize(1024);
    }

    MultiPolyTool::~MultiPolyTool() {  }

    void MultiPolyTool::disable() {
	reset();
	MultiWCTool::disable();
    }

    void MultiPolyTool::keyPressed(const WCPositionEvent &ev) {

	Int x = ev.pixX();
	Int y = ev.pixY();
	WorldCanvas *wc = ev.worldCanvas( );
	if ( ! wc->inDrawArea(x, y) ) return;

	its2ndLastPressTime = itsLastPressTime;
	itsLastPressTime = ev.timeOfEvent();
  
	// finish building the polygon...
	// could still be needed for flagging measurement sets...
	if (ev.timeOfEvent()-its2ndLastPressTime < doubleClickInterval())  {
	    if ( memory::nullptr.check(building_polygon) == false ) {
		building_polygon->closeFigure( );
		building_polygon = memory::nullptr;
		refresh( );
		return;
	    }
	    doubleClicked(x, y);
	}

	double linx1, liny1;
	viewer::screen_to_linear( wc, x, y, linx1, liny1 );

	// constructing a polygon...
	if ( memory::nullptr.check(building_polygon) == false ) {
	    building_polygon->addVertex( linx1, liny1, true );
	    building_polygon->addVertex( linx1, liny1 );
	    refresh( );
	    return;
	}

	// traverse in reverse order because the last region created is "on top"...
	// check for click within a handle...
	for ( polygonlist::reverse_iterator iter = polygons.rbegin(); iter != polygons.rend(); ++iter ) {
	    if ( (resizing_region_handle = (*iter)->clickHandle( linx1, liny1 )) != 0 ) {
		resizing_region = *iter;		// enter resizing state
		moving_regions.clear( );		// ensure that moving state is clear...
		return;
	    }
	}

	// check for click within one (or more) regions...
	resizing_region = memory::nullptr;
	moving_regions.clear( );			// ensure that moving state is clear...
	for ( polygonlist::iterator iter = polygons.begin(); iter != polygons.end(); ++iter ) {
	    if ( (*iter)->clickWithin( linx1, liny1 ) )
		moving_regions.push_back(*iter);
	}
	moving_linx_ = linx1;
	moving_liny_ = liny1;
	if ( moving_regions.size( ) > 0 ) return;

	// start new rectangle
	start_new_polygon( wc, x, y );		// enter resizing state
	polygonReady();
	return;
    }

#if OLDSTUFF
  clicked(x, y);

  if(itsMode==Def) {

    // (still) defining new polygon.

    if( ev.worldCanvas()!=itsCurrentWC || !itsCurrentWC->inDrawArea(x,y) )
        return;		// ignore these.

    if (inHandle(0, x, y) || inHandle(itsNPoints - 2, x, y)) {

      // 2nd press on first or last point--polygon ready

      popPoint();
      if(itsNPoints<3) {reset(); return;  }	// ignore <3 points
      itsMode = Ready;
      itsLastPressTime = its2ndLastPressTime = -1.0;
      refresh();
      polygonReady();  }
	// this callback is unused (and useless?) on glish level (12/01)

    else {
	// key pressed elsewhere - add point
	// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
	// note that points are added as the cursor is moved... thus
	//   (1) pop to remove the last "move" position
	//   (2) push to put on the actually "click" position
	//   (3) push again to provide a dummy to be removed later
	// I have no idea why the "move" points are pushed...  <drs>
	popPoint();
	pushPoint(x, y);
	pushPoint(x, y);  }
    return;  }     


  if(itsMode==Ready && ev.worldCanvas() == itsCurrentWC) {

    // Click on WC with previously defined polygon

    for (Int i = 0; i < itsNPoints; i++) if (inHandle(i, x, y)) {

      // user has pressed on a handle

      itsSelectedHandle = i;
      itsMode = Resize;
      refresh();
      return;  }

    if (inPolygon(x, y)) {

      // user has pressed inside the polygon

      itsMode = Move;
      itsBaseMoveX = x;
      itsBaseMoveY = y;
      refresh();
      return;  }

    // click outside polygon.

    // Next line disabled 5/07 dk.  Any click outside a defined polygon
    // will now start a new one, regardless of whether the poly has been 
    // double-clicked _after_ definition (polygonReady() _has_ been
    // emitted in any case).  Maintenance of itsEmitted is now superfluous.
 // if(!itsEmitted) return;
	// if polygon was already emitted, code below will erase it
	// and start a new one.
  }
  
  if(itsMode==Move || itsMode==Resize) return;
		// shouldn't happen; last button release should have
		// taken it out of Move or Resize state.
 

  // no previously existing polygon,
  // or click in a different WC,
  // or click outside a polygon already emitted:
  
  // Start new polygon

  if(itsMode!=Off) reset();	// erase old one, if any.
  itsCurrentWC = ev.worldCanvas();
  itsNPoints=0;
  pushPoint(x, y);
  pushPoint(x, y);
  itsMode = Def;
  polygonReady();
  return;
}
#endif

    void MultiPolyTool::moved(const WCMotionEvent &ev) {

	if (ev.worldCanvas() != itsCurrentWC) return;  // shouldn't happen

	Int x = ev.pixX();
	Int y = ev.pixY();
	if ( ! itsCurrentWC->inDrawArea(x, y) ) return;

	double linx, liny;
	viewer::screen_to_linear( itsCurrentWC, x, y, linx, liny );

	if ( memory::nullptr.check(building_polygon) == false ) {
	    building_polygon->addVertex( linx, liny, true );
	    refresh( );
	    return;
	}

	bool refresh_needed = false;
	bool region_selected = false;
	if ( memory::nullptr.check(resizing_region) == false ) {
	    // resize the rectangle
	    double linx1, liny1;
	    viewer::screen_to_linear( itsCurrentWC, x, y, linx1, liny1 );
	    resizing_region_handle = resizing_region->moveHandle( resizing_region_handle, linx1, liny1 );
	    refresh( );
	    // refresh_needed = true;
	    return;
	}

	for ( polygonlist::reverse_iterator iter = polygons.rbegin(); iter != polygons.rend(); ++iter ) {
	    unsigned int result = (*iter)->mouseMovement(linx,liny,region_selected);
	    refresh_needed = refresh_needed | viewer::Region::refreshNeeded(result);
	    region_selected = region_selected | viewer::Region::regionSelected(result);
	}

	if ( refresh_needed ) {
	    refresh( );
	}

	//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
	if ( moving_regions.size( ) > 0 ) {
	    // resize the rectangle
	    double linx1, liny1;
	    viewer::screen_to_linear( itsCurrentWC, x, y, linx1, liny1 );
	    double dx = linx1 - moving_linx_;
	    double dy = liny1 - moving_liny_;
	    for( polygonlist::iterator iter = moving_regions.begin( ); iter != moving_regions.end( ); ++iter ) {
		(*iter)->move( dx, dy );
	    }
	    moving_linx_ = linx1;
	    moving_liny_ = liny1;
	}
	//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  if(itsMode==Off || itsMode==Ready) return;
  if(!itsCurrentWC->inDrawArea(x,y)) return;

  if(itsMode == Def) {
    popPoint();		// pop existing moving position
    pushPoint(x, y);  }	// push new moving position

  else if(itsMode == Move) {
    Int dx = x-itsBaseMoveX,  dy = y-itsBaseMoveY;
    Vector<Int> pX, pY;
    get(pX, pY);
    pX = pX + dx;
    pY = pY + dy;
    set(pX, pY);	// move all the points by (dx,dy)
    itsBaseMoveX = x;
    itsBaseMoveY = y;  
    updateRegion(); }

  else if (itsMode == Resize) {
    set(x,y, itsSelectedHandle); // move selected vertex.
    updateRegion(); }

  itsEmitted = False;  // changed polygon => not yet emitted.
  refresh();  
}


    void MultiPolyTool::keyReleased(const WCPositionEvent &ev) {

	if ( memory::nullptr.check(resizing_region) == false ) {
	    // resize finished
	    resizing_region = memory::nullptr;
	}

	if ( moving_regions.size( ) > 0 ) {
	    // moving finished
	    moving_regions.clear( );
	}

  Bool needsHandles=False;
  if(itsMode==Move || itsMode==Resize) {
    itsMode=Ready;
    needsHandles=True;  }

  if (itsMode==Ready && ev.worldCanvas()==itsCurrentWC &&
      ev.timeOfEvent()-its2ndLastPressTime < doubleClickInterval() )  {
    Int x = ev.pixX();
    Int y = ev.pixY();

    if(itsCurrentWC->inDrawArea(x,y)) {

      // "double click" in draw area & polygon exists

      itsLastPressTime = its2ndLastPressTime = -1.0;
				// reset dbl click timing

      if (!itsPolygonPersistent) reset();
      else {
        itsEmitted = True;
        if(needsHandles) refresh();  }
		// vertices and WC still remain valid until next
		// polygon started. In particular, during callbacks below.

      if (inPolygon(x, y)) doubleInside();
      else doubleOutside();

      return;  }}

  if(needsHandles) {
    refresh();
    polygonReady();  }	}
	// this callback is unused (and useless?) on glish level (12/01)


    void MultiPolyTool::otherKeyPressed(const WCPositionEvent &ev) {
	const int pixel_step = 1;
	WorldCanvas *wc = ev.worldCanvas( );
	if ( wc == itsCurrentWC && 
	     ( ev.key() == Display::K_Escape ||
	       ev.key() == Display::K_Left ||
	       ev.key() == Display::K_Right ||
	       ev.key() == Display::K_Up ||
	       ev.key() == Display::K_Down ) ) {
	    uInt x = ev.pixX();
	    uInt y = ev.pixY();

	    resizing_region = memory::nullptr;
	    moving_regions.clear( );		// ensure that moving state is clear...

	    double linx, liny;
	    viewer::screen_to_linear( wc, x, y, linx, liny );

	    bool refresh_needed = false;
	    for ( polygonlist::iterator iter = polygons.begin(); iter != polygons.end(); ) {
		if ( (*iter)->regionVisible( ) ) {
		    unsigned int result = (*iter)->mouseMovement(linx,liny,false);
		    if ( viewer::Region::regionSelected(result) ) {
			if ( ev.key() == Display::K_Escape ) {
			    polygonlist::iterator xi = iter; ++xi;
			    polygons.erase( iter );
			    refresh_needed = true;
			    iter = xi;
			} else {
			    double dx=0, dy=0;
			    switch ( ev.key( ) ) {
				case Display::K_Left:
				    viewer::screen_offset_to_linear_offset( wc, -pixel_step, 0, dx, dy );
				    break;
				case Display::K_Right:
				    viewer::screen_offset_to_linear_offset( wc, pixel_step, 0, dx, dy );
				    break;
				case Display::K_Down:
				    viewer::screen_offset_to_linear_offset( wc, 0, -pixel_step, dx, dy );
				    break;
				case Display::K_Up:
				    viewer::screen_offset_to_linear_offset( wc, 0, pixel_step, dx, dy );
				    break;
				default:
				    break;
			    }
			    (*iter)->move( dx, dy );
			    refresh_needed = true;
			    ++iter;
			}
		    } else { ++iter; }
		} else { ++iter; }
	    }
	    if ( refresh_needed ) refresh( );
	}
    }

    void MultiPolyTool::draw(const WCRefreshEvent &ev) {
	for ( polygonlist::iterator iter = polygons.begin(); iter != polygons.end(); ++iter )
	    (*iter)->draw( );
    }


    void MultiPolyTool::revokeRegion( viewer::Region *p ) {
	viewer::Polygon *poly = dynamic_cast<viewer::Polygon*>(p);
	if ( poly == 0 ) return;

	for ( polygonlist::iterator iter = polygons.begin(); iter != polygons.end(); ++iter ) {
	    if ( (*iter).get( ) == poly ) {
		polygons.erase( iter );
		refresh( );
		break;
	    }
	}
    }

void MultiPolyTool::reset(Bool skipRefresh) {
  Bool existed = (itsMode!=Off);
  itsMode = Off;
  itsEmitted = False;
  itsLastPressTime = its2ndLastPressTime = -1.0;
  if(existed && !skipRefresh) refresh();  }	// erase old drawing if necessary.



void MultiPolyTool::get(Vector<Int> &x, Vector<Int> &y) const {
  if(!itsCurrentWC) return;
  x.resize(itsNPoints);
  y.resize(itsNPoints);
  Int ix, iy;
  for (Int i = 0; i < itsNPoints; i++) {
    get(ix,iy, i);
    x(i)=ix; y(i)=iy;  }  }

void MultiPolyTool::get(Int &x, Int &y, const Int pt) const {
  if(!itsCurrentWC || pt>=itsNPoints) return;
  Vector<Double> pix(2), lin(2);
  lin(0) = itsX(pt);
  lin(1) = itsY(pt);
  itsCurrentWC->linToPix(pix, lin);
  x = ifloor(pix(0) + 0.5);
  y = ifloor(pix(1) + 0.5);  }

void MultiPolyTool::set(const Vector<Int> &x, const Vector<Int> &y) {
  if (!itsCurrentWC) return;
  if(x.shape()<itsNPoints || y.shape()<itsNPoints) return;
  Int ix, iy;
  for (Int i = 0; i < itsNPoints; i++) {
    ix = x(i); iy = y(i);
    set(ix, iy, i);  }  }

void MultiPolyTool::set(const Int x, const Int y, const Int pt) {
  if(!itsCurrentWC || pt>=Int(itsX.nelements())) return;
  Vector<Double> pix(2), lin(2);
  pix(0) = x;
  pix(1) = y;
  itsCurrentWC->pixToLin(lin, pix);
  itsX(pt) = lin(0);
  itsY(pt) = lin(1);  }

void MultiPolyTool::pushPoint(Int x, Int y) {
  if (itsNPoints < Int(itsX.nelements())) {
    set(x,y, itsNPoints);
    itsNPoints++;  }  }

void MultiPolyTool::popPoint() {
  if (itsNPoints > 0) itsNPoints--;  }

Bool MultiPolyTool::inHandle(const Int &pt, const Int &x, 
			  const Int &y) const {
  if (pt<0 || pt >= itsNPoints) return False;

  Int ptx,pty;
  get(ptx,pty, pt);
  Int del = (itsHandleSize - 1) / 2;
  return (x >= ptx - del  &&  x <= ptx + del &&
	  y >= pty - del  &&  y <= pty + del);  }

Bool MultiPolyTool::inPolygon(const Int &x, const Int &y) const {
  Int nabove = 0, nbelow = 0; // counts of crossing lines above and below
  
  Vector<Int> pX, pY;
  get(pX, pY);
  Int i, j;
  for (i = 0; i < itsNPoints; i++) {
    if (i > 0) j = i - 1;
    else j = itsNPoints - 1;

    if (min(pX(j), pX(i)) < x && max(pX(j), pX(i)) > x) {
      Float ycut = (Float)pY(j) + (Float)(pY(i) - pY(j)) /
	(Float)(pX(i) - pX(j)) * (Float)(x - pX(j));
      if (ycut > (Float)y) nabove++;
      else nbelow++;  }  }

  if ((nabove + nbelow) % 2) return True;
    // not even - possibly on a line of the polygon.

  return (nabove % 2);  }


    void MultiPolyTool::checkPoint( WorldCanvas *wc, State &state ) {
	for ( polygonlist::iterator iter = ((MultiPolyTool*) this)->polygons.begin(); iter != polygons.end(); ++iter ) {
	    viewer::Region::PointInfo point_state = (*iter)->checkPoint( state.x( ), state.y( ) );
	    // should consider introducing a cptr_ref which somehow allows creating a
	    // base class reference based on a counted pointer to a derived class...
	    state.insert( this, &*(*iter), point_state );
	}
    }

    static std::set<viewer::RegionCreator::Types> multi_poly_tool_region_set;
    const std::set<viewer::RegionCreator::Types> &MultiPolyTool::regionsCreated( ) const {
	if ( multi_poly_tool_region_set.size( ) == 0 ) {
	    multi_poly_tool_region_set.insert( POLYGON );
	}
	return multi_poly_tool_region_set;
    }

    bool MultiPolyTool::create( Types region_type, WorldCanvas *wc, const std::vector<std::pair<double,double> > &pts, const std::string &label,
				const std::string &font, int font_size, int font_style, const std::string &font_color,
				const std::string &line_color, viewer::Region::LineStyle line_style ) {
	if ( pts.size( ) <= 2 ) return false;
	if ( itsCurrentWC == 0 ) itsCurrentWC = wc;
	std::tr1::shared_ptr<viewer::Polygon> result = (rfactory->polygon( wc, pts ));
	result->setLabel( label );
	result->setFont( font, font_size, font_style, font_color );
	result->setLine( line_color, line_style );
	polygons.push_back( result );
	refresh( );
	return true;
    }

    void MultiPolyTool::start_new_polygon( WorldCanvas *wc, int x, int y ) {

	// As originally requested by Kumar, any non-modified regions would be erased when a
	// new one was started, but Juergen said that new regions should not automatically cause
	// other regions to disappear... instead, there <ESC> should cause the selected regions
	// to be be deleted...      <drs>
	// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
        // for ( rectanglelist::iterator iter = rectangles.begin(); iter != rectangles.end(); ++iter ) {
	//     // the only change that will count as "modifying" is a non-zero length name
	//     if ( (*iter)->modified( ) == false ) {
	// 	rectangles.erase( iter );
	// 	continue;
	//     }
	// }

	itsCurrentWC = wc;

	double linx, liny;
	viewer::screen_to_linear( itsCurrentWC, x, y, linx, liny );
	building_polygon = (rfactory->polygon( wc, linx, liny ));
	building_polygon->addVertex(linx,liny);
	resizing_region_handle = 1;
	polygons.push_back( building_polygon );
	moving_regions.clear( );		// ensure that moving state is clear...
    }

} //# NAMESPACE CASA - END

