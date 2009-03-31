// Based on code/alma/apps/UVWCoords

#include <casa/Logging/LogIO.h>
#include <msvis/MSVis/MSUVWGenerator.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MPosition.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSAntennaColumns.h>
#include <measures/Measures/MCBaseline.h>

namespace casa {

// The UvwCoords ctor has lines for the antennas, antenna offsets, and station
// positions.  This ctor assumes they're present in msc_p if present at all.
  MSUVWGenerator::MSUVWGenerator(MS &ms_ref, const MBaseline::Types bltype,
				 const Muvw::Types uvwtype) :
  msc_p(ms_ref),				    	
  bl_csys_p(MBaseline::Ref(bltype)),           // MBaseline::J2000
  uvw_csys_p(uvwtype),                         // uvw_csys_p(Muvw::J2000)
  antColumns_p(msc_p.antenna()),
  antPositions_p(antColumns_p.positionMeas()),
  antOffset_p(antColumns_p.offsetMeas()),
  refpos_p(antPositions_p(0)),  // We use the first antenna for the reference
  feedOffset_p(msc_p.feed().positionMeas())
{
  fill_bl_an(bl_an_p, ms_ref);		
}

MSUVWGenerator::~MSUVWGenerator(){
}

void MSUVWGenerator::fill_bl_an(Vector<MVBaseline>& bl_an_p, const MS &ms_ref)
{
  nant_p = antPositions_p.table().nrow();

  Double max_baseline = -1.0;
  Double bl_len;

  const ROScalarColumn<Double>& antDiams = antColumns_p.dishDiameter();
  Double smallestDiam = antDiams(0);
  Double secondSmallestDiam = antDiams(0);
  
  bl_an_p.resize(nant_p);
  for(uInt an = 0; an < nant_p; ++an){
    bl_an_p[an] = MVBaseline(refpos_p.getValue(), antPositions_p(an).getValue());

    // MVBaseline has functions to return the length, but Manhattan distances
    // are good enough for this, and faster than a sqrt.
    Vector<Double> bluvw(bl_an_p[an].getValue());
    bl_len = fabs(bluvw[0]) + fabs(bluvw[1]) + fabs(bluvw[2]);

    if(bl_len > max_baseline)
      max_baseline = bl_len;

    if(antDiams(an) < secondSmallestDiam){
      if(antDiams(an) < smallestDiam){
	secondSmallestDiam = smallestDiam;
	smallestDiam = antDiams(an);
      }
      else
	secondSmallestDiam = antDiams(an);
    }
  }

  // Setting timeRes_p to 0.05 * the time for a 1 radian phase change on the
  // longest baseline at 2x the primary beamwidth should be sufficiently short
  // for Earth based observations.  Space-based baselines will move faster, but
  // probably don't have the data rate to support full beam imaging.
  timeRes_p = 0.05 * 24.0 * 3600.0 / (6.283 * 2.44) *
    sqrt(smallestDiam * secondSmallestDiam) / max_baseline;
}  

void MSUVWGenerator::uvw_an(const Double timeCentroid, const Int fldID)
{
  const MDirection& phasedir = msc_p.field().phaseDirMeas(fldID);
  MeasFrame  measFrame(refpos_p, MEpoch(Quantity(timeCentroid, "s"), MEpoch::TAI),
		       phasedir);
  MVBaseline mvbl;
  MBaseline  basMeas;

  // at ref ant, at timeCentroid, for the phaseDir
  MBaseline::Ref basref(MBaseline::ITRF, measFrame);
  basMeas.set(mvbl, basref);
  basMeas.getRefPtr()->set(measFrame);

  // convert from ITRF vector to baseline vector in bl_csys_p's frame (likely J2000).
  MBaseline::Convert elconv(basMeas, bl_csys_p);

  Muvw          uvwMeas;
  Muvw::Ref     uvwref(uvw_csys_p, measFrame);
  Muvw::Convert uvwconv(uvwMeas, uvwref);
 
  for(uInt i = 0; i < nant_p; ++i){
    //TODO: (Soon!) Antenna offsets are not handled yet.
    basMeas.set(bl_an_p[i], basref);
    MBaseline bas2000 = elconv(basMeas);
    MVuvw uvw2000(bas2000.getValue(), phasedir.getValue());
    
    antUVW_p[i] = uvw2000.getValue();
  }
}

// antUVW_p must be set up for the correct timeCentroid and phase direction by
// uvw_an() before calling this.
void MSUVWGenerator::uvw_bl(const uInt ant1, const uInt feed1,
			    const uInt ant2, const uInt feed2,
			    Array<Double>& uvw)
{
  //uvw.resize(3);      // Probably redundant.  Does it significantly slow things down?
  //TODO: Feed offsets are not handled yet.
  uvw = antUVW_p[ant2] - antUVW_p[ant1];
}

Bool MSUVWGenerator::make_uvws(const Vector<Int> flds)
{
  ArrayColumn<Double>&      UVWcol   = msc_p.uvw();
  const Vector<Double>&     timeCent = msc_p.timeCentroid().getColumn();
  const ROScalarColumn<Int> fieldID(msc_p.fieldId());
  const ROScalarColumn<Int> ant1(msc_p.antenna1());
  const ROScalarColumn<Int> ant2(msc_p.antenna2());
  const ROScalarColumn<Int> feed1(msc_p.feed1());
  const ROScalarColumn<Int> feed2(msc_p.feed2());

  // Use a time ordered index to minimize the number of calls to uvw_an.
  // TODO: use field as a secondary sort key.
  Vector<uInt> tOI;
  GenSortIndirect<Double>::sort(tOI, timeCent);

  // Having uvw_an() calculate positions for each antenna for every field is
  // somewhat inefficient since in a multiconfig MS not all antennas will be
  // used in each time interval, but it's not clear that determining which
  // antennas will be used for a given uvw_an() call would be any more
  // efficient.  It's not horribly inefficient, because uvw_an() is O(nant_p),
  // and uvw_bl() is only called for baselines that are actually used.
  antUVW_p.resize(nant_p);

  logSink() << LogOrigin("MSUVWGenerator", "make_uvws") << LogIO::NORMAL3;
  
  logSink() << LogIO::DEBUG1 << "timeRes_p: " << timeRes_p << LogIO::POST;

  Double oldTime = tOI[0] - 2.0 * timeRes_p;     // Ensure a call to uvw_an
  Int    oldFld  = -2;				 // on the 1st iteration.
  for(uInt row = 0; row < msc_p.nrow(); ++row){
    uInt toir = tOI[row];

    if(timeCent(toir) - oldTime > timeRes_p || fieldID(toir) != oldFld){
      oldTime = timeCent(toir);
      oldFld  = fieldID(toir);
      uvw_an(oldTime, oldFld);
    }
    
    try{
      if(flds[fieldID(toir)] > -1){
	//      uvw_bl(ant1(toir), ant2(toir),
	//     feed1(toir), feed2(toir), UVWcol(toir));
	UVWcol.put(toir, antUVW_p[ant2(toir)] - antUVW_p[ant1(toir)]);
      }
    }
    catch(AipsError x){
      logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
		<< LogIO::POST;
      throw(AipsError("Error in MSUVWGenerator::make_uvws."));
      return false;
    }
  }
  return true;
}

LogIO& MSUVWGenerator::logSink() {return sink_p;};

// void MSUVWGenerator::get_ant_offsets(const MDirection& dir_with_a_frame)
// {
//   // This appears to be a required column of the ANTENNA table in version 2.0
//   // of the MeasurementSet definition
//   // (http://aips2.nrao.edu/docs/notes/229/229.html), so it is assumed to be
//   // present.  However, it is usually a set of zeroes, based on the common
//   // belief that it is only needed for heterogeneous arrays, since the
//   // receivers of homogeneous arrays move in concert.  That is not true when
//   // there are independent pointing errors.

//   // Convert ant_offset_measures to Vectors and check for nonzeroness.
//   ant_offset_vec_p.resize(nant);
//   for(uInt n = 0; n < nant; ++n)
//     ant_offset_vec_p[n] = ant_offset_meas_p.convert(0, pointingdir).getValue();
// }

// Bool MSUVWGenerator::set_receiv_offsets(const MDirection& dir_with_a_frame)
// {
//   // This appears to be a required column of the FEED table in version 2.0
//   // of the MeasurementSet definition
//   // (http://aips2.nrao.edu/docs/notes/229/229.html), so it is assumed to be
//   // present.  However, it is usually a set of zeroes, based on the common
//   // belief that it is only needed for heterogeneous arrays, since the
//   // receivers of homogeneous arrays move in concert.  That is not true when
//   // there are independent pointing errors.

//   // Convert feed_offset_measures to Vectors and check for nonzeroness.
//   Vector<Vector<Double> > offsetvects;
//   offsetvects.resize(nant);
//   for(uInt n = 0; n < nant; ++n){
//     offsetvects[n] = ant_offsets->convert(0, pointingdir).getValue();
//     if(ant_offsets[n] != ant_offsets[0]){
//       varying_offsets = true;
//     }
//   }

//   ignore_offsets = true;
//   return ignore_offsets;
// }

Bool MSUVWGenerator::calc_ref_positions(const MDirection& pointingdir)
{    
  // ref_positions.resize(nant_p);
  //for(uInt n = 0; n < nant_p; ++n)
  //  ref_positions[n] = antPositions_p.convert(n, pointingdir);

  return false;  // TODO: Determine when the positions need to be updated, or
		 // if uvw_an supercedes this function.
}

} // Ends namespace casa.
