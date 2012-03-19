//# FlagDataHandler.h: This file contains the implementation of the FlagDataHandler class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <flagging/Flagging/FlagDataHandler.h>

namespace casa { //# NAMESPACE CASA - BEGIN

/////////////////////////////////////
/// FlagDataHandler implementation //
/////////////////////////////////////

// -----------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------
FlagDataHandler::FlagDataHandler(string msname, uShort iterationApproach, Double timeInterval):
		msname_p(msname), iterationApproach_p(iterationApproach)
{
	// Initialize logger
	logger_p = new LogIO(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Set time interval
	setTimeInterval(timeInterval);

	// Deactivate profiling by default
	profiling_p = false;

	// Disable async I/O by default
	enableAsyncIO(false);

	// Check if slurp is enabled
	if (!asyncio_enabled_p)
	{
		slurp_p = true;
		AipsrcValue<Bool>::find (slurp_p,"FlagDataHandler.slurp", true);
	}
	else
	{
		slurp_p = false;
	}

	// WARNING: By default the visibility iterator adds the following
	// default columns: ARRAY_ID and FIELD_ID,DATA_DESC_ID and TIME.
	// And they are needed for the correct operation of the VisibilityIterator
	// (it needs to know when any of these changes to be able to give
	// the correct coordinates with the data). If these columns are not
	// explicitly sorted on, they will be added BEFORE any others, with
	// unexpected iteration behaviour
	// See: MSIter::construct

	// WARNING: By specifying Time as a sort column we are not producing
	// chunks with only one time step, but just the other way around!
	// (all time steps are grouped in the same chunk). This is because TIME
	// is one of the 'implicit' sort columns, and as explained before it is
	// added BEFORE the ones specified by the user, resulting in chunks with
	// only one time step, thus invalidating the time interval effect.
	// See: MSIter.h

	// By default we map polarizations because they are needed for the per-chunk logs
	mapPolarizations_p = true;

	// Antenna pointing map is only needed by the Elevation agent and I have move it there
	mapAntennaPointing_p = false;

	// These mapping is for the Quack agent, but requires a complete MS iteration, so I put it here,
	// together with the full swap that I do to pre-compute the amount of memory necessary to run
	mapScanStartStop_p = false;
	mapScanStartStopFlagged_p = false;

	// Initialize Pre-Load columns
	preLoadColumns_p.clear();
	preLoadColumns_p.push_back(VisBufferComponents::FieldId);
	preLoadColumns_p.push_back(VisBufferComponents::SpW);
	preLoadColumns_p.push_back(VisBufferComponents::Scan);
	preLoadColumns_p.push_back(VisBufferComponents::ArrayId);
	preLoadColumns_p.push_back(VisBufferComponents::ObservationId);

	preLoadColumns_p.push_back(VisBufferComponents::NRow);
	preLoadColumns_p.push_back(VisBufferComponents::NChannel);
	preLoadColumns_p.push_back(VisBufferComponents::NCorr);

	// Set the iteration approach based on the agent
	setIterationApproach(iterationApproach);

	// Initialize selection ranges
	anySelection_p = false;
	timeSelection_p = String("");
	baselineSelection_p = String("");
	fieldSelection_p = String("");
	// NOTE (after Dic 2011 testing): As far as I know spw selection does not have to be *
	// (can be empty) and in fact applying a spw selection slows down the MSSelection class
	spwSelection_p = String("");
	uvwSelection_p = String("");
	polarizationSelection_p = String("");
	scanSelection_p = String("");
	arraySelection_p = String("");
	observationSelection_p = String("");
	scanIntentSelection_p = String("");

	// Initialize iteration parameters
	chunksInitialized_p = false;
	buffersInitialized_p = false;
	iteratorGenerated_p = false;
	stopIteration_p = false;
	maxChunkRows = 0;
	chunkNo = 0;
	bufferNo = 0;

	// Initialize stats
	stats_p = false;
	cubeAccessCounter_p = 0;
	chunkCounts_p = 0;
	msCounts_p = 0;

	// Set all the initialized pointers to NULL
	measurementSetSelection_p = NULL;
	visibilityBuffer_p = NULL;
	antennaNames_p = NULL;
	antennaPositions_p = NULL;
	antennaDiameters_p = NULL;
	antennaPairMap_p = NULL;
	subIntegrationMap_p = NULL;
	corrProducts_p = NULL;
	polarizationMap_p = NULL;
	polarizationIndexMap_p = NULL;
	antennaPointingMap_p = NULL;
	scanStartStopMap_p = NULL;
	fieldNames_p = NULL;

	// Initialize FlagDataHanler-FlagAgents state
	flushFlags_p = false;
	flushFlagRow_p = false;

	return;
}


// -----------------------------------------------------------------------
// Default destructor
// -----------------------------------------------------------------------
FlagDataHandler::~FlagDataHandler()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));
	*logger_p << LogIO::DEBUG1 << "FlagDataHandler::~FlagDataHandler()" << LogIO::POST;

	// Delete logger
	if (logger_p) delete logger_p;

	// Delete MS objects
	if (measurementSetSelection_p) delete measurementSetSelection_p;

	// Delete mapping members
	if (antennaNames_p) delete antennaNames_p;
	if (antennaPositions_p) delete antennaPositions_p;
	if (antennaDiameters_p) delete antennaDiameters_p;
	if (antennaPairMap_p) delete antennaPairMap_p;
	if (subIntegrationMap_p) delete subIntegrationMap_p;
	if (corrProducts_p) delete corrProducts_p;
	if (polarizationMap_p) delete polarizationMap_p;
	if (polarizationIndexMap_p) delete polarizationIndexMap_p;
	if (antennaPointingMap_p) delete antennaPointingMap_p;
	if (scanStartStopMap_p) delete scanStartStopMap_p;
	if (fieldNames_p) delete fieldNames_p;

	return;
}


// -----------------------------------------------------------------------
// Set iteration Approach
// -----------------------------------------------------------------------
void
FlagDataHandler::setIterationApproach(uShort iterationApproach)
{
	iterationApproach_p = iterationApproach;

	switch (iterationApproach_p)
	{
		case COMPLETE_SCAN_MAPPED:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMPLETE_SCAN_MAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = true;
			setMapAntennaPairs(true);
			setMapSubIntegrations(true);
			break;
		}
		case COMPLETE_SCAN_MAP_SUB_INTEGRATIONS_ONLY:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMPLETE_SCAN_MAP_SUB_INTEGRATIONS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = true;
			setMapSubIntegrations(true);
			break;
		}
		case COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = false;
			setMapAntennaPairs(true);
			break;
		}
		case COMPLETE_SCAN_UNMAPPED:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMPLETE_SCAN_UNMAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case COMBINE_SCANS_MAPPED:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMBINE_SCANS_MAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = true;
			setMapAntennaPairs(true);
			setMapSubIntegrations(true);
			break;
		}
		case COMBINE_SCANS_MAP_SUB_INTEGRATIONS_ONLY:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMBINE_SCANS_MAP_SUB_INTEGRATIONS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = true;
			setMapSubIntegrations(true);
			break;
		}
		case COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = false;
			setMapAntennaPairs(true);
			break;
		}
		case COMBINE_SCANS_UNMAPPED:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMBINE_SCANS_UNMAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case ANTENNA_PAIR:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: ANTENNA_PAIR" << LogIO::POST;
			sortOrder_p = Block<int>(8);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			// NOTE: As requested by Urvashi, in this way we produce chunks
			// per antenna pair with all the time steps grouped instead
			// of chunks with entire integrations for a given time step.
			sortOrder_p[5] = MS::ANTENNA1;
			sortOrder_p[6] = MS::ANTENNA2;
			sortOrder_p[7] = MS::TIME;

			// NOTE: groupTimeSteps_p=true groups all time steps together in one buffer.
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case  SUB_INTEGRATION:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: SUB_INTEGRATION" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = false;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case  ARRAY_FIELD:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: ARRAY_FIELD" << LogIO::POST;
			sortOrder_p = Block<int>(4);
			sortOrder_p[0] = MS::ARRAY_ID;
			sortOrder_p[1] = MS::FIELD_ID;
			sortOrder_p[2] = MS::DATA_DESC_ID;
			sortOrder_p[3] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = false;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		default:
		{
			*logger_p << LogIO::DEBUG1 << " Iteration mode: COMPLETE_SCAN_UNMAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
	}

	// We have changed the iteration approach to the iterators has to be regenerated
	iteratorGenerated_p = false;
	chunksInitialized_p = false;
	buffersInitialized_p = false;
	stopIteration_p = false;

	return;
}

// -----------------------------------------------------------------------
// Mapping functions as requested by Urvashi
// -----------------------------------------------------------------------
void
FlagDataHandler::generateAntennaPairMap()
{
	logger_p->origin(LogOrigin("FlagMSHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (antennaPairMap_p) delete antennaPairMap_p;
	antennaPairMap_p = new antennaPairMap();

	// Retrieve antenna vectors
	Vector<Int> antenna1Vector = visibilityBuffer_p->get()->antenna1();
	Vector<Int> antenna2Vector = visibilityBuffer_p->get()->antenna2();

	// Fill map
	Int ant1_i,ant2_i;
	uInt nRows = antenna1Vector.size();
	for (uInt row_idx=0;row_idx<nRows;row_idx++)
	{
		ant1_i = antenna1Vector[row_idx];
		ant2_i = antenna2Vector[row_idx];
		if (antennaPairMap_p->find(std::make_pair(ant1_i,ant2_i)) == antennaPairMap_p->end())
		{
			std::vector<uInt> newPair;
			newPair.push_back(row_idx);
			(*antennaPairMap_p)[std::make_pair(ant1_i,ant2_i)] = newPair;
		}
		else
		{
			(*antennaPairMap_p)[std::make_pair(ant1_i,ant2_i)].push_back(row_idx);
		}
	}
	*logger_p << LogIO::DEBUG1 <<  " " << antennaPairMap_p->size() <<" Antenna pairs found in current buffer" << LogIO::POST;

	return;
}


void
FlagDataHandler::generateSubIntegrationMap()
{
	logger_p->origin(LogOrigin("FlagMSHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (subIntegrationMap_p) delete subIntegrationMap_p;
	subIntegrationMap_p = new subIntegrationMap();

	// Retrieve antenna vectors
	Vector<Double> timeVector = visibilityBuffer_p->get()->time();

	// Fill map
	uInt nRows = timeVector.size();
	for (uInt row_idx=0;row_idx<nRows;row_idx++)
	{
		if (subIntegrationMap_p->find(timeVector[row_idx]) == subIntegrationMap_p->end())
		{
			std::vector<uInt> newSubIntegration;
			newSubIntegration.push_back(row_idx);
			(*subIntegrationMap_p)[timeVector[row_idx]] = newSubIntegration;
		}
		else
		{
			(*subIntegrationMap_p)[timeVector[row_idx]].push_back(row_idx);
		}
	}
	*logger_p << LogIO::NORMAL <<  " " << subIntegrationMap_p->size() <<" Sub-Integrations (time steps) found in current buffer" << LogIO::POST;

	return;
}


void
FlagDataHandler::generatePolarizationsMap()
{
	logger_p->origin(LogOrigin("FlagMSHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (polarizationMap_p) delete polarizationMap_p;
	polarizationMap_p = new polarizationMap();
	if (polarizationIndexMap_p) delete polarizationIndexMap_p;
	polarizationIndexMap_p = new polarizationIndexMap();

	uShort pos = 0;
	Vector<Int> corrTypes = visibilityBuffer_p->get()->corrType();
	*logger_p << LogIO::DEBUG1 << " Correlation type: " <<  corrTypes << LogIO::POST;

	for (Vector<Int>::iterator iter = corrTypes.begin(); iter != corrTypes.end();iter++)
	{
		switch (*iter)
		{
			case Stokes::I:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is I" << LogIO::POST;
				(*polarizationMap_p)[Stokes::I] = pos;
				(*polarizationIndexMap_p)[pos] = "I";
				break;
			}
			case Stokes::Q:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is Q" << LogIO::POST;
				(*polarizationMap_p)[Stokes::Q] = pos;
				(*polarizationIndexMap_p)[pos] = "Q";
				break;
			}
			case Stokes::U:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is U" << LogIO::POST;
				(*polarizationMap_p)[Stokes::U] = pos;
				(*polarizationIndexMap_p)[pos] = "U";
				break;
			}
			case Stokes::V:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is V" << LogIO::POST;
				(*polarizationMap_p)[Stokes::V] = pos;
				(*polarizationIndexMap_p)[pos] = "V";
				break;
			}
			case Stokes::XX:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is XX" << LogIO::POST;
				(*polarizationMap_p)[Stokes::XX] = pos;
				(*polarizationIndexMap_p)[pos] = "XX";
				break;
			}
			case Stokes::YY:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is YY" << LogIO::POST;
				(*polarizationMap_p)[Stokes::YY] = pos;
				(*polarizationIndexMap_p)[pos] = "YY";
				break;
			}
			case Stokes::XY:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is XY" << LogIO::POST;
				(*polarizationMap_p)[Stokes::XY] = pos;
				(*polarizationIndexMap_p)[pos] = "XY";
				break;
			}
			case Stokes::YX:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is YX" << LogIO::POST;
				(*polarizationMap_p)[Stokes::YX] = pos;
				(*polarizationIndexMap_p)[pos] = "YX";
				break;
			}
			case Stokes::RR:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is RR" << LogIO::POST;
				(*polarizationMap_p)[Stokes::RR] = pos;
				(*polarizationIndexMap_p)[pos] = "RR";
				break;
			}
			case Stokes::LL:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is LL" << LogIO::POST;
				(*polarizationMap_p)[Stokes::LL] = pos;
				(*polarizationIndexMap_p)[pos] = "LL";
				break;
			}
			case Stokes::RL:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is RL" << LogIO::POST;
				(*polarizationMap_p)[Stokes::RL] = pos;
				(*polarizationIndexMap_p)[pos] = "RL";
				break;
			}
			case Stokes::LR:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is LR" << LogIO::POST;
				(*polarizationMap_p)[Stokes::LR] = pos;
				(*polarizationIndexMap_p)[pos] = "LR";
				break;
			}
			case VisMapper::CALSOL1:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th calibration solution" << LogIO::POST;
				(*polarizationMap_p)[VisMapper::CALSOL1] = pos;
				(*polarizationIndexMap_p)[pos] = "Sol1";
				break;
			}
			case VisMapper::CALSOL2:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th calibration solution" << LogIO::POST;
				(*polarizationMap_p)[VisMapper::CALSOL2] = pos;
				(*polarizationIndexMap_p)[pos] = "Sol2";
				break;
			}
			case VisMapper::CALSOL3:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th calibration solution" << LogIO::POST;
				(*polarizationMap_p)[VisMapper::CALSOL3] = pos;
				(*polarizationIndexMap_p)[pos] = "Sol3";
				break;
			}
			case VisMapper::CALSOL4:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th calibration solution" << LogIO::POST;
				(*polarizationMap_p)[VisMapper::CALSOL4] = pos;
				(*polarizationIndexMap_p)[pos] = "Sol4";
				break;
			}
			default:
			{
				*logger_p << LogIO::WARN << " The " << pos << " th correlation is unknown: " << *iter << LogIO::POST;
				break;
			}
		}
		pos++;
	}

	for (polarizationMap::iterator iter =polarizationMap_p->begin();iter != polarizationMap_p->end();iter++)
	{
		*logger_p << LogIO::DEBUG1 << " Polarization map key: " << iter->first << " value: " << iter->second << LogIO::POST;
	}

	return;
}

void
FlagDataHandler::generateAntennaPointingMap()
{
	logger_p->origin(LogOrigin("FlagMSHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (antennaPointingMap_p) delete antennaPointingMap_p;
	antennaPointingMap_p = new antennaPointingMap();

	Vector<Double> time = visibilityBuffer_p->get()->time();
	uInt nRows = time.size();
	antennaPointingMap_p->reserve(nRows);
	for (uInt row_i=0;row_i<nRows;row_i++)
	{
		Vector<MDirection> azimuth_elevation = visibilityBuffer_p->get()->azel(time[row_i]);
		Int ant1 = visibilityBuffer_p->get()->antenna1()[row_i];
		Int ant2 = visibilityBuffer_p->get()->antenna1()[row_i];

	    double antenna1_elevation = azimuth_elevation[ant1].getAngle("deg").getValue()[1];
	    double antenna2_elevation = azimuth_elevation[ant2].getAngle("deg").getValue()[1];

	    vector<Double> item(2);
	    item[0] = antenna1_elevation;
	    item[1] = antenna2_elevation;
	    antennaPointingMap_p->push_back(item);
	}

	*logger_p << LogIO::NORMAL << " Generated antenna pointing map with "
			<< antennaPointingMap_p->size() << " elements" << LogIO::POST;

	return;
}


// -----------------------------------------------------------------------
// Generate scan start stop map
// -----------------------------------------------------------------------
void
FlagDataHandler::generateScanStartStopMap()
{
	return;
}


// -----------------------------------------------------------------------
// Set Data Selection parameters
// -----------------------------------------------------------------------
bool
FlagDataHandler::setDataSelection(Record record)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	int exists;

	exists = record.fieldNumber ("array");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("array"), arraySelection_p);

		if (arraySelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " array selection is " << arraySelection_p << LogIO::POST;
		}
	}
	else
	{
		arraySelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no array selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("field");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("field"), fieldSelection_p);

		if (fieldSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " field selection is " << fieldSelection_p << LogIO::POST;
		}
	}
	else
	{
		fieldSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no field selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("scan");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("scan"), scanSelection_p);

		if (scanSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " scan selection is " << scanSelection_p << LogIO::POST;
		}
	}
	else
	{
		scanSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no scan selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("timerange");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("timerange"), timeSelection_p);

		if (timeSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " timerange selection is " << timeSelection_p << LogIO::POST;
		}
	}
	else
	{
		timeSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no timerange selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("spw");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("spw"), spwSelection_p);

		if (spwSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " spw selection is " << spwSelection_p << LogIO::POST;
		}
	}
	else
	{
		spwSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no spw selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("antenna");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("antenna"), baselineSelection_p);

		if (baselineSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " antenna selection is " << baselineSelection_p << LogIO::POST;
		}
	}
	else
	{
		baselineSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no antenna selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("uvrange");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("uvrange"), uvwSelection_p);

		if (uvwSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " uvrange selection is " << uvwSelection_p << LogIO::POST;
		}
	}
	else
	{
		uvwSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no uvrange selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("correlation");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("correlation"), polarizationSelection_p);

		if (polarizationSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " correlation selection is " << polarizationSelection_p << LogIO::POST;
		}
	}
	else
	{
		polarizationSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no correlation selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("observation");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("observation"), observationSelection_p);

		if (observationSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " observation selection is " << observationSelection_p << LogIO::POST;
		}
	}
	else
	{
		observationSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no observation selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("intent");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("intent"), scanIntentSelection_p);

		if (scanIntentSelection_p.size())
		{
			*logger_p << LogIO::DEBUG1 << " scan intent selection is " << scanIntentSelection_p << LogIO::POST;
		}
	}
	else
	{
		scanIntentSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no scan intent selection" << LogIO::POST;
	}

	return true;
}


// -----------------------------------------------------------------------
// Set time interval
// -----------------------------------------------------------------------
void
FlagDataHandler::setTimeInterval(Double timeInterval)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));
	if (timeInterval >= 0)
	{
		timeInterval_p = timeInterval;
		*logger_p << LogIO::DEBUG1 << "Set time interval to " << timeInterval_p << "s"<<LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::WARN << "Provided time interval is negative: " <<  timeInterval << LogIO::POST;
	}

	return;
}


// -----------------------------------------------------------------------
// Enable async i/o
// -----------------------------------------------------------------------
void
FlagDataHandler::enableAsyncIO(Bool enable)
{
	if (enable)
	{
		// Check if async i/o is enabled (double check for ROVisibilityIteratorAsync and FlagDataHandler config)
		asyncio_enabled_p = ROVisibilityIterator::isAsynchronousIoEnabled();

		if (asyncio_enabled_p)
		{
			// Check Flag Data Handler config
			Bool tmp = false;
			Bool foundSetting = AipsrcValue<Bool>::find (tmp,"FlagDataHandler.asyncio", false);
			tmp = ! foundSetting || tmp; // let global setting rule if no FlagDataHandler setting
			if (!tmp)
			{
				asyncio_enabled_p = false;
				*logger_p << LogIO::WARN << " Asyncio disabled from FlagDataHandler .casarc settings" << LogIO::POST;
			}
		}
		else
		{
			*logger_p << LogIO::WARN << " Asyncio disabled from VisibilityIterator .casarc settings" << LogIO::POST;
		}
	}
	else
	{
		asyncio_enabled_p = false;
	}


	if (!asyncio_enabled_p)
	{
		slurp_p = true;
		AipsrcValue<Bool>::find (slurp_p,"FlagDataHandler.slurp", true);
	}
	else
	{
		slurp_p = false;
	}

	return;
}


// -----------------------------------------------------------------------
// Function to handled columns pre-load (to avoid problems with parallelism)
// -----------------------------------------------------------------------
void
FlagDataHandler::preLoadColumn(uInt column)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	if (std::find (preLoadColumns_p.begin(), preLoadColumns_p.end(), column) == preLoadColumns_p.end())
	{
		*logger_p << LogIO::DEBUG1 << " Adding column to list: " <<  column << LogIO::POST;
		preLoadColumns_p.push_back(column);
	}

	return;
}


// -----------------------------------------------------------------------
// Fill pre-fetch columns
// -----------------------------------------------------------------------
void
FlagDataHandler::preFetchColumns()
{
	for (vector<uInt>::iterator iter=preLoadColumns_p.begin();iter!=preLoadColumns_p.end();iter++)
	{
		switch (*iter)
		{
			case VisBufferComponents::Ant1:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Ant1);
				}
				else
				{
					visibilityBuffer_p->get()->antenna1();
				}
				break;
			}
			case VisBufferComponents::Ant2:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Ant2);
				}
				else
				{
					visibilityBuffer_p->get()->antenna2();
				}
				break;
			}
			case VisBufferComponents::ArrayId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ArrayId);
				}
				else
				{
					visibilityBuffer_p->get()->arrayId();
				}
				break;
			}
			case VisBufferComponents::Channel:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Channel);
				}
				else
				{
					visibilityBuffer_p->get()->channel();
				}
				break;
			}
			case VisBufferComponents::Cjones:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Cjones);
				}
				else
				{
					visibilityBuffer_p->get()->CJones();
				}
				break;
			}
			case VisBufferComponents::CorrType:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::CorrType);
				}
				else
				{
					visibilityBuffer_p->get()->corrType();
				}
				break;
			}
			case VisBufferComponents::Corrected:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Corrected);
				}
				else
				{
					visibilityBuffer_p->get()->correctedVisibility();
				}
				break;
			}
			case VisBufferComponents::CorrectedCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::CorrectedCube);
				}
				else
				{
					visibilityBuffer_p->get()->correctedVisCube();
				}
				break;
			}
			case VisBufferComponents::Direction1:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Direction1);
				}
				else
				{
					visibilityBuffer_p->get()->direction1();
				}
				break;
			}
			case VisBufferComponents::Direction2:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Direction2);
				}
				else
				{
					visibilityBuffer_p->get()->direction2();
				}
				break;
			}
			case VisBufferComponents::Exposure:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Exposure);
				}
				else
				{
					visibilityBuffer_p->get()->exposure();
				}
				break;
			}
			case VisBufferComponents::Feed1:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed1);
				}
				else
				{
					visibilityBuffer_p->get()->feed1();
				}
				break;
			}
			case VisBufferComponents::Feed1_pa:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed1_pa);
				}
				else
				{
					visibilityBuffer_p->get()->feed1_pa();
				}
				break;
			}
			case VisBufferComponents::Feed2:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed2);
				}
				else
				{
					visibilityBuffer_p->get()->feed2();
				}
				break;
			}
			case VisBufferComponents::Feed2_pa:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed2_pa);
				}
				else
				{
					visibilityBuffer_p->get()->feed2_pa();
				}
				break;
			}
			case VisBufferComponents::FieldId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FieldId);
				}
				else
				{
					visibilityBuffer_p->get()->fieldId();
				}
				break;
			}
			case VisBufferComponents::Flag:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Flag);
				}
				else
				{
					visibilityBuffer_p->get()->flag();
				}
				break;
			}
			case VisBufferComponents::FlagCategory:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FlagCategory);
				}
				else
				{
					visibilityBuffer_p->get()->flagCategory();
				}
				break;
			}
			case VisBufferComponents::FlagCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FlagCube);
				}
				else
				{
					visibilityBuffer_p->get()->flagCube();
				}
				break;
			}
			case VisBufferComponents::FlagRow:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FlagRow);
				}
				else
				{
					visibilityBuffer_p->get()->flagRow();
				}
				break;
			}
			case VisBufferComponents::Freq:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Freq);
				}
				else
				{
					visibilityBuffer_p->get()->frequency();
				}
				break;
			}
			case VisBufferComponents::ImagingWeight:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ImagingWeight);
				}
				else
				{
					visibilityBuffer_p->get()->imagingWeight();
				}
				break;
			}
			case VisBufferComponents::Model:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Model);
				}
				else
				{
					visibilityBuffer_p->get()->modelVisibility();
				}
				break;
			}
			case VisBufferComponents::ModelCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ModelCube);
				}
				else
				{
					visibilityBuffer_p->get()->modelVisCube();
				}
				break;
			}
			case VisBufferComponents::NChannel:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::NChannel);
				}
				else
				{
					visibilityBuffer_p->get()->nChannel();
				}
				break;
			}
			case VisBufferComponents::NCorr:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::NCorr);
				}
				else
				{
					visibilityBuffer_p->get()->nCorr();
				}
				break;
			}
			case VisBufferComponents::NRow:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::NRow);
				}
				else
				{
					visibilityBuffer_p->get()->nRow();
				}
				break;
			}
			case VisBufferComponents::ObservationId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ObservationId);
				}
				else
				{
					visibilityBuffer_p->get()->observationId();
				}
				break;
			}
			case VisBufferComponents::Observed:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Observed);
				}
				else
				{
					visibilityBuffer_p->get()->visibility();
				}
				break;
			}
			case VisBufferComponents::ObservedCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ObservedCube);
				}
				else
				{
					visibilityBuffer_p->get()->visCube();
				}
				break;
			}
			case VisBufferComponents::PhaseCenter:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::PhaseCenter);
				}
				else
				{
					visibilityBuffer_p->get()->phaseCenter();
				}
				break;
			}
			case VisBufferComponents::PolFrame:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::PolFrame);
				}
				else
				{
					visibilityBuffer_p->get()->polFrame();
				}
				break;
			}
			case VisBufferComponents::ProcessorId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ProcessorId);
				}
				else
				{
					visibilityBuffer_p->get()->processorId();
				}
				break;
			}
			case VisBufferComponents::Scan:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Scan);
				}
				else
				{
					visibilityBuffer_p->get()->scan();
				}
				break;
			}
			case VisBufferComponents::Sigma:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Sigma);
				}
				else
				{
					visibilityBuffer_p->get()->sigma();
				}
				break;
			}
			case VisBufferComponents::SigmaMat:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::SigmaMat);
				}
				else
				{
					visibilityBuffer_p->get()->sigmaMat();
				}
				break;
			}
			case VisBufferComponents::SpW:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::SpW);
				}
				else
				{
					visibilityBuffer_p->get()->spectralWindow();
				}
				break;
			}
			case VisBufferComponents::StateId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::StateId);
				}
				else
				{
					visibilityBuffer_p->get()->stateId();
				}
				break;
			}
			case VisBufferComponents::Time:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Time);
				}
				else
				{
					visibilityBuffer_p->get()->time();
				}
				break;
			}
			case VisBufferComponents::TimeCentroid:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::TimeCentroid);
				}
				else
				{
					visibilityBuffer_p->get()->timeCentroid();
				}
				break;
			}
			case VisBufferComponents::TimeInterval:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::TimeInterval);
				}
				else
				{
					visibilityBuffer_p->get()->timeInterval();
				}
				break;
			}
			case VisBufferComponents::Weight:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Weight);
				}
				else
				{
					visibilityBuffer_p->get()->weight();
				}
				break;
			}
			case VisBufferComponents::WeightMat:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::WeightMat);
				}
				else
				{
					visibilityBuffer_p->get()->weightMat();
				}
				break;
			}
			case VisBufferComponents::WeightSpectrum:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::WeightSpectrum);
				}
				else
				{
					visibilityBuffer_p->get()->weightSpectrum();
				}
				break;
			}
			case VisBufferComponents::Uvw:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Uvw);
				}
				else
				{
					visibilityBuffer_p->get()->uvw();
				}
				break;
			}
			case VisBufferComponents::UvwMat:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::UvwMat);
				}
				else
				{
					visibilityBuffer_p->get()->uvwMat();
				}
				break;
			}
		}
	}

	return;
}


// -----------------------------------------------------------------------
// Methods to trigger generation of different maps
// -----------------------------------------------------------------------
void
FlagDataHandler::setMapAntennaPairs(bool activated)
{
	mapAntennaPairs_p=activated;
	// Pre-Load antenna1, antenna2
	preLoadColumn(VisBufferComponents::Ant1);
	preLoadColumn(VisBufferComponents::Ant2);
}


void
FlagDataHandler::setMapSubIntegrations(bool activated)
{
	mapSubIntegrations_p=activated;
	// Pre-Load time
	preLoadColumn(VisBufferComponents::Time);
}


void
FlagDataHandler::setMapPolarizations(bool activated)
{
	mapPolarizations_p=activated;
	// Pre-Load corrType
	preLoadColumn(VisBufferComponents::CorrType);
}


void
FlagDataHandler::setMapAntennaPointing(bool activated)
{
	// Antenna Pointing mapping is now done by the first
	// Elevation agent, and stored as static variable
	// mapAntennaPointing_p=activated;

	// Pre-Load time, antenna1 and antenna2
	// Azel is derived and this only restriction
	// is that it can be access by 1 thread only
	preLoadColumn(VisBufferComponents::Time);
	preLoadColumn(VisBufferComponents::Ant1);
	preLoadColumn(VisBufferComponents::Ant2);
}


void
FlagDataHandler::setScanStartStopMap(bool activated)
{
	mapScanStartStop_p=activated;
	// Pre-Load scan and time
	preLoadColumn(VisBufferComponents::Scan);
	preLoadColumn(VisBufferComponents::Time);
}


void
FlagDataHandler::setScanStartStopFlaggedMap(bool activated)
{
	mapScanStartStopFlagged_p=activated;
	// Pre-Load scan and time
	preLoadColumn(VisBufferComponents::Scan);
	preLoadColumn(VisBufferComponents::Time);
}


/////////////////////////////////////
/// VisMapper implementation ////////
/////////////////////////////////////
VisMapper::VisMapper(String expression,polarizationMap *polMap,CubeView<Complex> *leftVis,CubeView<Complex> *rightVis)
{
	rightVis_p = NULL;
	leftVis_p = NULL;
	setParentCubes(leftVis,rightVis);
	setExpressionMapping(expression,polMap);
}


VisMapper::VisMapper(String expression,polarizationMap *polMap)
{
	rightVis_p = NULL;
	leftVis_p = NULL;
	setExpressionMapping(expression,polMap);
}


void
VisMapper::setParentCubes(CubeView<Complex> *leftVis,CubeView<Complex> *rightVis)
{
	if (rightVis_p != NULL) delete rightVis_p;
	if (leftVis_p != NULL) delete leftVis_p;

	leftVis_p = leftVis;
	IPosition leftVisSize = leftVis->shape(); // pol,chan,row
	reducedLength_p = IPosition(3);
	reducedLength_p(0) = leftVisSize(1); // chan
	reducedLength_p(1) = leftVisSize(2); // row
	if (selectedCorrelationProducts_p.size() > 0)
	{
		reducedLength_p(2) = selectedCorrelations_p.size(); //pols
	}
	else
	{
		reducedLength_p(2) = leftVisSize(1); //pols
	}


	if (rightVis != NULL)
	{
		rightVis_p = rightVis;
		getVis_p = &VisMapper::diffVis;
	}
	else
	{
		rightVis_p = NULL;
		getVis_p = &VisMapper::leftVis;
	}
}


void
VisMapper::setExpressionMapping(String expression,polarizationMap *polMap)
{
	selectedCorrelationProducts_p.clear();
	selectedCorrelations_p.clear();
	selectedCorrelationStrings_p.clear();
	expression_p = expression;
	polMap_p = polMap;
	bool matchExpression = false;

	// Parse complex unitary function
	if (expression_p.find("REAL") != string::npos)
	{
		applyVisExpr_p = &VisMapper::real;
	}
	else if (expression_p.find("IMAG") != string::npos)
	{
		applyVisExpr_p = &VisMapper::imag;
	}
	else if (expression_p.find("ARG") != string::npos)
	{
		applyVisExpr_p = &VisMapper::arg;
	}
	else if (expression_p.find("ABS") != string::npos)
	{
		applyVisExpr_p = &VisMapper::abs;
	}
	else if (expression_p.find("NORM") != string::npos)
	{
		applyVisExpr_p = &VisMapper::norm;
	}
	else
	{
		applyVisExpr_p = &VisMapper::abs;
	}

	// Parse polarization
	if (expression_p.find("WVR") != string::npos)
	{
		if (polMap_p->find(Stokes::I) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::I]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_i);
			selectedCorrelationStrings_p.push_back(string("I"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (WVR - I) not available");
		}
	}
	if (expression_p.find("XX") != string::npos)
	{
		if (polMap_p->find(Stokes::XX) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::XX]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::linear_xx);
			selectedCorrelationStrings_p.push_back(string("XX"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (XX) not available");
		}
	}
	if (expression_p.find("YY") != string::npos)
	{
		if (polMap_p->find(Stokes::YY) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::YY]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::linear_yy);
			selectedCorrelationStrings_p.push_back(string("YY"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (YY) not available");
		}
	}
	if (expression_p.find("XY") != string::npos)
	{
		if (polMap_p->find(Stokes::XY) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::XY]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::linear_xy);
			selectedCorrelationStrings_p.push_back(string("XY"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (XY) not available");
		}
	}
	if (expression_p.find("YX") != string::npos)
	{
		if (polMap_p->find(Stokes::YX) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::YX]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::linear_yx);
			selectedCorrelationStrings_p.push_back(string("YX"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (YX) not available");
		}
	}
	if (expression_p.find("RR") != string::npos)
	{
		if (polMap_p->find(Stokes::RR) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::RR]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::circular_rr);
			selectedCorrelationStrings_p.push_back(string("RR"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (RR) not available");
		}
	}
	if (expression_p.find("LL") != string::npos)
	{
		if (polMap_p->find(Stokes::LL) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::LL]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::circular_ll);
			selectedCorrelationStrings_p.push_back(string("LL"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (LL) not available");
		}
	}
	if (expression_p.find("LR") != string::npos)
	{
		if (polMap_p->find(Stokes::LR) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::LR]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::circular_lr);
			selectedCorrelationStrings_p.push_back(string("LR"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (LR) not available");
		}
	}
	if (expression_p.find("RL") != string::npos)
	{
		if (polMap_p->find(Stokes::RL) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::RL]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::circular_rl);
			selectedCorrelationStrings_p.push_back(string("RL"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (RL) not available");
		}
	}
	if (expression_p.find("I") != string::npos)
	{
		if (polMap_p->find(Stokes::I) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::I]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_i);
			selectedCorrelationStrings_p.push_back(string("I"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XX) != polMap_p->end()) and (polMap_p->find(Stokes::YY) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::XX]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::YY]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_i_from_linear);
			selectedCorrelationStrings_p.push_back(string("I = (XX+YY)/2"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RR) != polMap_p->end()) and (polMap_p->find(Stokes::LL) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::RR]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::LL]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_i_from_circular);
			selectedCorrelationStrings_p.push_back(string("I = (RR+LL)/2"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (I) cannot be computed from available polarizations");
		}
	}
	if (expression_p.find("Q") != string::npos)
	{
		if (polMap_p->find(Stokes::Q) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::Q]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_q);
			selectedCorrelationStrings_p.push_back(string("Q"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XX) != polMap_p->end()) and (polMap_p->find(Stokes::YY) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::XX]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::YY]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_q_from_linear);
			selectedCorrelationStrings_p.push_back(string("Q = (XX-YY)/2"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RL) != polMap_p->end()) and (polMap_p->find(Stokes::LR) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::RL]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::LR]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_q_from_circular);
			selectedCorrelationStrings_p.push_back(string("Q = (RL-LR)/2"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (Q) cannot be computed from available polarizations");
		}
	}
	if (expression_p.find("U") != string::npos)
	{
		if (polMap_p->find(Stokes::U) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::U]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_u);
			selectedCorrelationStrings_p.push_back(string("U"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XY) != polMap_p->end()) and (polMap_p->find(Stokes::YX) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::XY]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::YX]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_u_from_linear);
			selectedCorrelationStrings_p.push_back(string("U = (XY-YX)/2"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RL) != polMap_p->end()) and (polMap_p->find(Stokes::LR) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::RL]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::LR]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_u_from_circular);
			selectedCorrelationStrings_p.push_back(string(" U = (RL-LR)/2i"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (U) cannot be computed from available polarizations");
		}
	}
	if ((expression_p.find("V") != string::npos) and (expression_p.find("WVR") == string::npos))
	{
		if (polMap_p->find(Stokes::V) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::V]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_v);
			selectedCorrelationStrings_p.push_back(string("V"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XY) != polMap_p->end()) and (polMap_p->find(Stokes::YX) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::XY]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::YX]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_v_from_linear);
			selectedCorrelationStrings_p.push_back(string("V = (XY-YX)/2i"));
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RR) != polMap_p->end()) and (polMap_p->find(Stokes::LL) != polMap_p->end()))
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[Stokes::RR]);
			selectedPolarizations.push_back((*polMap_p)[Stokes::LL]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::stokes_v_from_circular);
			selectedCorrelationStrings_p.push_back(string("V = (RR-LL)/2"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (V) cannot be computed from available polarizations");
		}
	}
	if (expression_p.find("Sol1") != string::npos)
	{
		if (polMap_p->find(VisMapper::CALSOL1) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[VisMapper::CALSOL1]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::calsol1);
			selectedCorrelationStrings_p.push_back(string("Sol1"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Calibration solution element (Sol1) not available");
		}
	}
	if (expression_p.find("Sol2") != string::npos)
	{
		if (polMap_p->find(VisMapper::CALSOL2) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[VisMapper::CALSOL2]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::calsol2);
			selectedCorrelationStrings_p.push_back(string("Sol2"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Calibration solution element (Sol1) not available");
		}
	}
	if (expression_p.find("Sol3") != string::npos)
	{
		if (polMap_p->find(VisMapper::CALSOL3) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[VisMapper::CALSOL3]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::calsol3);
			selectedCorrelationStrings_p.push_back(string("Sol3"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Calibration solution element (Sol1) not available");
		}
	}
	if (expression_p.find("Sol4") != string::npos)
	{
		if (polMap_p->find(VisMapper::CALSOL4) != polMap_p->end())
		{
			vector<uInt> selectedPolarizations;
			selectedPolarizations.push_back((*polMap_p)[VisMapper::CALSOL4]);
			selectedCorrelations_p.push_back(selectedPolarizations);
			selectedCorrelationProducts_p.push_back(&VisMapper::calsol4);
			selectedCorrelationStrings_p.push_back(string("Sol4"));
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Calibration solution element (Sol1) not available");
		}
	}


	if (!matchExpression)
	{
		throw AipsError("Unknown polarization requested, (" + expression_p + ") supported types are: XX,YY,XY,YX,RR,LL,RL,LR,I,Q,U,V");
	}
}


VisMapper::~VisMapper()
{
	if (rightVis_p != NULL) delete rightVis_p;
	if (leftVis_p != NULL) delete leftVis_p;
}


Float
VisMapper::operator()(uInt chan, uInt row)
{
	Complex val = (*this.*selectedCorrelationProducts_p[0])(chan,row);
	return (*this.*applyVisExpr_p)(val);
}


Float
VisMapper::operator()(uInt pol, uInt chan, uInt row)
{
	Complex val = (*this.*selectedCorrelationProducts_p[pol])(chan,row);
	return (*this.*applyVisExpr_p)(val);
}


Complex
VisMapper::correlationProduct(uInt pol, uInt chan, uInt row)
{
	return (*this.*selectedCorrelationProducts_p[pol])(chan,row);
}


Complex
VisMapper::leftVis(uInt pol, uInt chan, uInt row)
{
	return leftVis_p->operator()(pol,chan,row);
}


Complex
VisMapper::diffVis(uInt pol, uInt chan, uInt row)
{
	return leftVis_p->operator()(pol,chan,row)-rightVis_p->operator()(pol,chan,row);
}


Complex
VisMapper::stokes_i(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::I],chan,row);
}


Complex
VisMapper::stokes_q(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::Q],chan,row);
}


Complex
VisMapper::stokes_u(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::U],chan,row);
}


Complex
VisMapper::stokes_v(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::V],chan,row);
}


Complex
VisMapper::linear_xx(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::XX],chan,row);
}


Complex
VisMapper::linear_yy(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::YY],chan,row);
}


Complex
VisMapper::linear_xy(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::XY],chan,row);
}


Complex
VisMapper::linear_yx(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::YX],chan,row);
}


Complex
VisMapper::circular_rr(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::RR],chan,row);
}


Complex
VisMapper::circular_ll(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::LL],chan,row);
}


Complex
VisMapper::circular_rl(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::RL],chan,row);
}


Complex
VisMapper::circular_lr(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::LR],chan,row);
}


Complex
VisMapper::stokes_i_from_linear(uInt chan, uInt row)
{
	// I = (XX+YY)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XX],chan,row) + (*this.*getVis_p)((*polMap_p)[Stokes::YY],chan,row))/2;
}


Complex
VisMapper::stokes_q_from_linear(uInt chan, uInt row)
{
	// Q = (XX-YY)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XX],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::YY],chan,row))/2;
}


Complex
VisMapper::stokes_u_from_linear(uInt chan, uInt row)
{
	// U = (XY-YX)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XY],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::YX],chan,row))/2;
}


Complex
VisMapper::stokes_v_from_linear(uInt chan, uInt row)
{
	// V = (XY-YX)/2i
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XY],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::YX],chan,row))/(2*ImaginaryUnit);
}


Complex
VisMapper::stokes_i_from_circular(uInt chan, uInt row)
{
	// I = (RR+LL)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RR],chan,row) + (*this.*getVis_p)((*polMap_p)[Stokes::LL],chan,row))/2;
}


Complex
VisMapper::stokes_q_from_circular(uInt chan, uInt row)
{
	// Q = (RL-LR)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RL],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::LR],chan,row))/2;
}


Complex
VisMapper::stokes_u_from_circular(uInt chan, uInt row)
{
	// U = (RL-LR)/2i
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RL],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::LR],chan,row))/(2*ImaginaryUnit);
}


Complex
VisMapper::stokes_v_from_circular(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::RR],chan,row);
}

Complex
VisMapper::calsol1(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[VisMapper::CALSOL1],chan,row);
}


Complex
VisMapper::calsol2(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[VisMapper::CALSOL2],chan,row);
}


Complex
VisMapper::calsol3(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[VisMapper::CALSOL3],chan,row);
}


Complex
VisMapper::calsol4(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[VisMapper::CALSOL4],chan,row);
}


//////////////////////////////////////
/// FlagMapper implementation ////////
//////////////////////////////////////
FlagMapper::FlagMapper(Bool flag, 	vector< vector<uInt> > selectedCorrelations,
									CubeView<Bool> *commonFlagsView,
									CubeView<Bool> *originalFlagsView,
									CubeView<Bool> *privateFlagsView,
									VectorView<Bool> *commonFlagRowView,
									VectorView<Bool> *originalFlagRowView,
									VectorView<Bool> *privateFlagRowView)
{
	commonFlagsView_p = NULL;
	originalFlagsView_p = NULL;
	privateFlagsView_p = NULL;
	commonFlagRowView_p = NULL;
	originalFlagRowView_p = NULL;
	privateFlagRowView_p = NULL;
	setParentCubes(commonFlagsView,originalFlagsView,privateFlagsView);
	setParentFlagRow(commonFlagRowView,originalFlagRowView,privateFlagRowView);
	setExpressionMapping(selectedCorrelations);
	nSelectedCorrelations_p = selectedCorrelations.size();
	flag_p = flag;
}


FlagMapper::FlagMapper(Bool flag, vector< vector<uInt> > selectedCorrelations)
{
	commonFlagsView_p = NULL;
	originalFlagsView_p = NULL;
	privateFlagsView_p = NULL;
	commonFlagRowView_p = NULL;
	originalFlagRowView_p = NULL;
	privateFlagRowView_p = NULL;
	setExpressionMapping(selectedCorrelations);
	nSelectedCorrelations_p = selectedCorrelations.size();
	flag_p = flag;
}


void
FlagMapper::setParentCubes(CubeView<Bool> *commonFlagsView,CubeView<Bool> *originalFlagsView,CubeView<Bool> *privateFlagsView)
{
	if (commonFlagsView_p != NULL) delete commonFlagsView_p;
	if (originalFlagsView_p != NULL) delete originalFlagsView_p;
	if (privateFlagsView_p != NULL) delete privateFlagsView_p;

	commonFlagsView_p = commonFlagsView;
	originalFlagsView_p = originalFlagsView;
	if (privateFlagsView != NULL)
	{
		privateFlagsView_p = privateFlagsView;
		applyFlag_p = &FlagMapper::applyPrivateFlags;
	}
	else
	{
		applyFlag_p = &FlagMapper::applyCommonFlags;
	}

	IPosition commonFlagCubeSize = commonFlagsView_p->shape(); // pol,chan,row
	reducedLength_p = IPosition(2);
	reducedLength_p(0) = commonFlagCubeSize(1); // chan
	reducedLength_p(1) = commonFlagCubeSize(2); // row

	if (nSelectedCorrelations_p>0)
	{
		reducedLength_p(2) = nSelectedCorrelations_p; // pol
	}
	else
	{
		reducedLength_p(2) = reducedLength_p(0);// pol
	}

	flagsPerRow_p = reducedLength_p(2)*reducedLength_p(0);
}


void
FlagMapper::setParentFlagRow(VectorView<Bool> *commonFlagRowView,VectorView<Bool> *originalFlagRowView,VectorView<Bool> *privateFlagRowView)
{
	if (commonFlagRowView_p != NULL) delete commonFlagRowView_p;
	if (originalFlagRowView_p != NULL) delete originalFlagRowView_p;
	if (privateFlagRowView_p != NULL) delete privateFlagRowView_p;

	commonFlagRowView_p = commonFlagRowView;
	originalFlagRowView_p = originalFlagRowView;
	if (privateFlagRowView_p != NULL)
	{
		privateFlagRowView_p = privateFlagRowView;
		applyFlagRow_p = &FlagMapper::applyCommonFlagRow;
	}
	else
	{
		applyFlagRow_p = &FlagMapper::applyPrivateFlagRow;
	}

	return;
}


void
FlagMapper::setExpressionMapping(vector< vector<uInt> > selectedCorrelations)
{
	selectedCorrelations_p = selectedCorrelations;
	if (selectedCorrelations_p.empty())
	{
		throw AipsError("None of the correlations is eligible for flagging");
	}
}


FlagMapper::~FlagMapper()
{
	if (commonFlagsView_p != NULL) delete commonFlagsView_p;
	if (originalFlagsView_p != NULL) delete originalFlagsView_p;
	if (privateFlagsView_p != NULL) delete privateFlagsView_p;
	if (commonFlagRowView_p != NULL) delete commonFlagRowView_p;
	if (originalFlagRowView_p != NULL) delete originalFlagRowView_p;
	if (privateFlagRowView_p != NULL) delete privateFlagRowView_p;
}


Bool
FlagMapper::getOriginalFlags(uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p[0].begin();iter!=selectedCorrelations_p[0].end();iter++)
	{
		combinedFlag = combinedFlag | originalFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}


Bool
FlagMapper::getModifiedFlags(uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p[0].begin();iter!=selectedCorrelations_p[0].end();iter++)
	{
		combinedFlag = combinedFlag | commonFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}


Bool
FlagMapper::getPrivateFlags(uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p[0].begin();iter!=selectedCorrelations_p[0].end();iter++)
	{
		combinedFlag = combinedFlag | privateFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}


Bool
FlagMapper::getOriginalFlags(uInt pol, uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p[pol].begin();iter!=selectedCorrelations_p[pol].end();iter++)
	{
		combinedFlag = combinedFlag | originalFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}


Bool
FlagMapper::getModifiedFlags(uInt pol, uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p[pol].begin();iter!=selectedCorrelations_p[pol].end();iter++)
	{
		combinedFlag = combinedFlag | commonFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}


Bool
FlagMapper::getPrivateFlags(uInt pol, uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p[pol].begin();iter!=selectedCorrelations_p[pol].end();iter++)
	{
		combinedFlag = combinedFlag | privateFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}


Bool
FlagMapper::getOriginalFlagRow(uInt row)
{
	return originalFlagRowView_p->operator ()(row);
}


Bool
FlagMapper::getModifiedFlagRow(uInt row)
{
	return commonFlagRowView_p->operator ()(row);
}


Bool
FlagMapper::getPrivateFlagRow(uInt row)
{
	return privateFlagRowView_p->operator ()(row);
}


void
FlagMapper::setModifiedFlags(uInt pol, uInt channel, uInt row)
{
	for (vector<uInt>::iterator iter=selectedCorrelations_p[pol].begin();iter!=selectedCorrelations_p[pol].end();iter++)
	{
		commonFlagsView_p->operator ()(*iter,channel,row) = flag_p;
	}
}


void
FlagMapper::setPrivateFlags(uInt pol, uInt channel, uInt row)
{
	for (vector<uInt>::iterator iter=selectedCorrelations_p[pol].begin();iter!=selectedCorrelations_p[pol].end();iter++)
	{
		privateFlagsView_p->operator ()(*iter,channel,row) = flag_p;
	}
}


void
FlagMapper::applyFlag(uInt channel, uInt row)
{
	for (vector<uInt>::iterator iter=selectedCorrelations_p[0].begin();iter!=selectedCorrelations_p[0].end();iter++)
	{
		(*this.*applyFlag_p)(*iter,channel,row);
	}
}


void
FlagMapper::applyFlag(uInt pol, uInt channel, uInt row)
{
	for (vector<uInt>::iterator iter=selectedCorrelations_p[pol].begin();iter!=selectedCorrelations_p[pol].end();iter++)
	{
		(*this.*applyFlag_p)(*iter,channel,row);
	}
}


void
FlagMapper::applyFlagRow(uInt row)
{
	// Flag cube
	for (uInt chan_i=0;chan_i<reducedLength_p(0);chan_i++)
	{
		for (vector< vector<uInt> >::iterator correlations=selectedCorrelations_p.begin();correlations!=selectedCorrelations_p.end();correlations++)
		{
			for (vector<uInt>::iterator iter=correlations->begin();iter!=correlations->end();iter++)
			{
				(*this.*applyFlag_p)(*iter,chan_i,row);
			}
		}
	}

	// Flag row
	(*this.*applyFlagRow_p)(row);
}


void
FlagMapper::applyCommonFlags(uInt pol, uInt channel, uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagsView_p->operator()(pol,channel,row) = flag_p;
	return;
}


void
FlagMapper::applyPrivateFlags(uInt pol, uInt channel, uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagsView_p->operator()(pol,channel,row) = flag_p;
	privateFlagsView_p->operator()(pol,channel,row) = flag_p;
	return;
}


void
FlagMapper::checkCommonFlags(uInt pol, uInt channel, uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	if (commonFlagsView_p->operator()(pol,channel,row) != flag_p)
	{
		cerr << "FlagMapper::" << __FUNCTION__ <<  " Flag missmatch at pol=" << pol << ",channel=" << channel << ",row=" << row << endl;
	}
	return;
}


void
FlagMapper::applyCommonFlagRow(uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagRowView_p->operator()(row) = flag_p;
	return;
}


void
FlagMapper::applyPrivateFlagRow(uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagRowView_p->operator()(row) = flag_p;
	originalFlagRowView_p->operator()(row) = flag_p;
	return;
}

} //# NAMESPACE CASA - END

