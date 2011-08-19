/*******************************************************************************
 * ALMA - Atacama Large Millimiter Array
 * (c) Institut de Radioastronomie Millimetrique, 2009
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * "@(#) $Id: ATMWVRMeasurement.cpp,v 1.5 2010/02/19 02:07:58 dbroguie Exp $"
 *
 * who       when      what
 * --------  --------  ----------------------------------------------
 * pardo     24/03/09  created
 */

#include "ATMWVRMeasurement.h"

#include <stdio.h>
#include <iostream>

using namespace std;

ATM_NAMESPACE_BEGIN

WVRMeasurement::WVRMeasurement()
{
}

WVRMeasurement::WVRMeasurement(const Angle &elevation,
                               const vector<Temperature> &measuredSkyBrightness)
{
  elevation_ = elevation;
  v_measuredSkyBrightness_ = measuredSkyBrightness;
  retrievedWaterVaporColumn_ = Length(-999, "mm"); // !< Retrieved zenith water vapor column for each event
  sigma_fittedSkyBrightness_ = Temperature(-999, "K"); // !< Sigma on the fitted sky brightness temperatures (average sigma over the WVR channels for each event).
  for(unsigned int i = 0; i < v_measuredSkyBrightness_.size(); i++) {
    v_fittedSkyBrightness_.push_back(Temperature(-999, "K"));
  } // !< Fitted sky brightness temperatures over all WVR channels for each event
}
WVRMeasurement::WVRMeasurement(const Angle &elevation,
                               const vector<Temperature> &measuredSkyBrightness,
                               const vector<Temperature> &fittedSkyBrightness,
                               const Length &retrievedWaterVaporColumn,
                               const Temperature &sigma_fittedSkyBrightness)
{
  elevation_ = elevation;
  v_measuredSkyBrightness_ = measuredSkyBrightness;
  v_fittedSkyBrightness_ = fittedSkyBrightness;
  retrievedWaterVaporColumn_ = retrievedWaterVaporColumn; // !< Retrieved zenith water vapor column for each event
  sigma_fittedSkyBrightness_ = sigma_fittedSkyBrightness; // !< Sigma on the fitted sky brightness temperatures (average sigma over the WVR channels for each event).
}

WVRMeasurement::~WVRMeasurement()
{
}

ATM_NAMESPACE_END
