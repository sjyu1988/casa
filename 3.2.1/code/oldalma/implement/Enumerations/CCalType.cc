
/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 * 
 * /////////////////////////////////////////////////////////////////
 * // WARNING!  DO NOT MODIFY THIS FILE!                          //
 * //  ---------------------------------------------------------  //
 * // | This is generated code!  Do not modify this file.       | //
 * // | Any changes will be lost when the file is re-generated. | //
 * //  ---------------------------------------------------------  //
 * /////////////////////////////////////////////////////////////////
 *
 * File CCalType.cpp
 */
#include <sstream>
#include <CCalType.h>
#include <string>
using namespace std;

	
const std::string& CCalType::sCAL_AMPLI = "CAL_AMPLI";
	
const std::string& CCalType::sCAL_ATMOSPHERE = "CAL_ATMOSPHERE";
	
const std::string& CCalType::sCAL_BANDPASS = "CAL_BANDPASS";
	
const std::string& CCalType::sCAL_CURVE = "CAL_CURVE";
	
const std::string& CCalType::sCAL_DELAY = "CAL_DELAY";
	
const std::string& CCalType::sCAL_FLUX = "CAL_FLUX";
	
const std::string& CCalType::sCAL_FOCUS = "CAL_FOCUS";
	
const std::string& CCalType::sCAL_FOCUS_MODEL = "CAL_FOCUS_MODEL";
	
const std::string& CCalType::sCAL_GAIN = "CAL_GAIN";
	
const std::string& CCalType::sCAL_HOLOGRAPHY = "CAL_HOLOGRAPHY";
	
const std::string& CCalType::sCAL_PHASE = "CAL_PHASE";
	
const std::string& CCalType::sCAL_POINTING = "CAL_POINTING";
	
const std::string& CCalType::sCAL_POINTING_MODEL = "CAL_POINTING_MODEL";
	
const std::string& CCalType::sCAL_POSITION = "CAL_POSITION";
	
const std::string& CCalType::sCAL_PRIMARY_BEAM = "CAL_PRIMARY_BEAM";
	
const std::string& CCalType::sCAL_SEEING = "CAL_SEEING";
	
const std::string& CCalType::sCAL_WVR = "CAL_WVR";
	
const std::vector<std::string> CCalType::sCalTypeSet() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_AMPLI);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_ATMOSPHERE);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_BANDPASS);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_CURVE);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_DELAY);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_FLUX);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_FOCUS);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_FOCUS_MODEL);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_GAIN);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_HOLOGRAPHY);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_PHASE);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_POINTING);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_POINTING_MODEL);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_POSITION);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_PRIMARY_BEAM);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_SEEING);
    
    enumSet.insert(enumSet.end(), CCalType::sCAL_WVR);
        
    return enumSet;
}

	

	
	
const std::string& CCalType::hCAL_AMPLI = "";
	
const std::string& CCalType::hCAL_ATMOSPHERE = "";
	
const std::string& CCalType::hCAL_BANDPASS = "";
	
const std::string& CCalType::hCAL_CURVE = "";
	
const std::string& CCalType::hCAL_DELAY = "";
	
const std::string& CCalType::hCAL_FLUX = "";
	
const std::string& CCalType::hCAL_FOCUS = "";
	
const std::string& CCalType::hCAL_FOCUS_MODEL = "";
	
const std::string& CCalType::hCAL_GAIN = "";
	
const std::string& CCalType::hCAL_HOLOGRAPHY = "";
	
const std::string& CCalType::hCAL_PHASE = "";
	
const std::string& CCalType::hCAL_POINTING = "";
	
const std::string& CCalType::hCAL_POINTING_MODEL = "";
	
const std::string& CCalType::hCAL_POSITION = "";
	
const std::string& CCalType::hCAL_PRIMARY_BEAM = "";
	
const std::string& CCalType::hCAL_SEEING = "";
	
const std::string& CCalType::hCAL_WVR = "";
	
const std::vector<std::string> CCalType::hCalTypeSet() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_AMPLI);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_ATMOSPHERE);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_BANDPASS);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_CURVE);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_DELAY);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_FLUX);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_FOCUS);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_FOCUS_MODEL);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_GAIN);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_HOLOGRAPHY);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_PHASE);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_POINTING);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_POINTING_MODEL);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_POSITION);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_PRIMARY_BEAM);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_SEEING);
    
    enumSet.insert(enumSet.end(), CCalType::hCAL_WVR);
        
    return enumSet;
}
   	

std::string CCalType::name(const CalTypeMod::CalType& f) {
    switch (f) {
    
    case CalTypeMod::CAL_AMPLI:
      return CCalType::sCAL_AMPLI;
    
    case CalTypeMod::CAL_ATMOSPHERE:
      return CCalType::sCAL_ATMOSPHERE;
    
    case CalTypeMod::CAL_BANDPASS:
      return CCalType::sCAL_BANDPASS;
    
    case CalTypeMod::CAL_CURVE:
      return CCalType::sCAL_CURVE;
    
    case CalTypeMod::CAL_DELAY:
      return CCalType::sCAL_DELAY;
    
    case CalTypeMod::CAL_FLUX:
      return CCalType::sCAL_FLUX;
    
    case CalTypeMod::CAL_FOCUS:
      return CCalType::sCAL_FOCUS;
    
    case CalTypeMod::CAL_FOCUS_MODEL:
      return CCalType::sCAL_FOCUS_MODEL;
    
    case CalTypeMod::CAL_GAIN:
      return CCalType::sCAL_GAIN;
    
    case CalTypeMod::CAL_HOLOGRAPHY:
      return CCalType::sCAL_HOLOGRAPHY;
    
    case CalTypeMod::CAL_PHASE:
      return CCalType::sCAL_PHASE;
    
    case CalTypeMod::CAL_POINTING:
      return CCalType::sCAL_POINTING;
    
    case CalTypeMod::CAL_POINTING_MODEL:
      return CCalType::sCAL_POINTING_MODEL;
    
    case CalTypeMod::CAL_POSITION:
      return CCalType::sCAL_POSITION;
    
    case CalTypeMod::CAL_PRIMARY_BEAM:
      return CCalType::sCAL_PRIMARY_BEAM;
    
    case CalTypeMod::CAL_SEEING:
      return CCalType::sCAL_SEEING;
    
    case CalTypeMod::CAL_WVR:
      return CCalType::sCAL_WVR;
    	
    }
    return std::string("");
}

	

	
std::string CCalType::help(const CalTypeMod::CalType& f) {
    switch (f) {
    
    case CalTypeMod::CAL_AMPLI:
      return CCalType::hCAL_AMPLI;
    
    case CalTypeMod::CAL_ATMOSPHERE:
      return CCalType::hCAL_ATMOSPHERE;
    
    case CalTypeMod::CAL_BANDPASS:
      return CCalType::hCAL_BANDPASS;
    
    case CalTypeMod::CAL_CURVE:
      return CCalType::hCAL_CURVE;
    
    case CalTypeMod::CAL_DELAY:
      return CCalType::hCAL_DELAY;
    
    case CalTypeMod::CAL_FLUX:
      return CCalType::hCAL_FLUX;
    
    case CalTypeMod::CAL_FOCUS:
      return CCalType::hCAL_FOCUS;
    
    case CalTypeMod::CAL_FOCUS_MODEL:
      return CCalType::hCAL_FOCUS_MODEL;
    
    case CalTypeMod::CAL_GAIN:
      return CCalType::hCAL_GAIN;
    
    case CalTypeMod::CAL_HOLOGRAPHY:
      return CCalType::hCAL_HOLOGRAPHY;
    
    case CalTypeMod::CAL_PHASE:
      return CCalType::hCAL_PHASE;
    
    case CalTypeMod::CAL_POINTING:
      return CCalType::hCAL_POINTING;
    
    case CalTypeMod::CAL_POINTING_MODEL:
      return CCalType::hCAL_POINTING_MODEL;
    
    case CalTypeMod::CAL_POSITION:
      return CCalType::hCAL_POSITION;
    
    case CalTypeMod::CAL_PRIMARY_BEAM:
      return CCalType::hCAL_PRIMARY_BEAM;
    
    case CalTypeMod::CAL_SEEING:
      return CCalType::hCAL_SEEING;
    
    case CalTypeMod::CAL_WVR:
      return CCalType::hCAL_WVR;
    	
    }
    return std::string("");
}
   	

CalTypeMod::CalType CCalType::newCalType(const std::string& name) {
		
    if (name == CCalType::sCAL_AMPLI) {
        return CalTypeMod::CAL_AMPLI;
    }
    	
    if (name == CCalType::sCAL_ATMOSPHERE) {
        return CalTypeMod::CAL_ATMOSPHERE;
    }
    	
    if (name == CCalType::sCAL_BANDPASS) {
        return CalTypeMod::CAL_BANDPASS;
    }
    	
    if (name == CCalType::sCAL_CURVE) {
        return CalTypeMod::CAL_CURVE;
    }
    	
    if (name == CCalType::sCAL_DELAY) {
        return CalTypeMod::CAL_DELAY;
    }
    	
    if (name == CCalType::sCAL_FLUX) {
        return CalTypeMod::CAL_FLUX;
    }
    	
    if (name == CCalType::sCAL_FOCUS) {
        return CalTypeMod::CAL_FOCUS;
    }
    	
    if (name == CCalType::sCAL_FOCUS_MODEL) {
        return CalTypeMod::CAL_FOCUS_MODEL;
    }
    	
    if (name == CCalType::sCAL_GAIN) {
        return CalTypeMod::CAL_GAIN;
    }
    	
    if (name == CCalType::sCAL_HOLOGRAPHY) {
        return CalTypeMod::CAL_HOLOGRAPHY;
    }
    	
    if (name == CCalType::sCAL_PHASE) {
        return CalTypeMod::CAL_PHASE;
    }
    	
    if (name == CCalType::sCAL_POINTING) {
        return CalTypeMod::CAL_POINTING;
    }
    	
    if (name == CCalType::sCAL_POINTING_MODEL) {
        return CalTypeMod::CAL_POINTING_MODEL;
    }
    	
    if (name == CCalType::sCAL_POSITION) {
        return CalTypeMod::CAL_POSITION;
    }
    	
    if (name == CCalType::sCAL_PRIMARY_BEAM) {
        return CalTypeMod::CAL_PRIMARY_BEAM;
    }
    	
    if (name == CCalType::sCAL_SEEING) {
        return CalTypeMod::CAL_SEEING;
    }
    	
    if (name == CCalType::sCAL_WVR) {
        return CalTypeMod::CAL_WVR;
    }
    
    throw badString(name);
}

CalTypeMod::CalType CCalType::literal(const std::string& name) {
		
    if (name == CCalType::sCAL_AMPLI) {
        return CalTypeMod::CAL_AMPLI;
    }
    	
    if (name == CCalType::sCAL_ATMOSPHERE) {
        return CalTypeMod::CAL_ATMOSPHERE;
    }
    	
    if (name == CCalType::sCAL_BANDPASS) {
        return CalTypeMod::CAL_BANDPASS;
    }
    	
    if (name == CCalType::sCAL_CURVE) {
        return CalTypeMod::CAL_CURVE;
    }
    	
    if (name == CCalType::sCAL_DELAY) {
        return CalTypeMod::CAL_DELAY;
    }
    	
    if (name == CCalType::sCAL_FLUX) {
        return CalTypeMod::CAL_FLUX;
    }
    	
    if (name == CCalType::sCAL_FOCUS) {
        return CalTypeMod::CAL_FOCUS;
    }
    	
    if (name == CCalType::sCAL_FOCUS_MODEL) {
        return CalTypeMod::CAL_FOCUS_MODEL;
    }
    	
    if (name == CCalType::sCAL_GAIN) {
        return CalTypeMod::CAL_GAIN;
    }
    	
    if (name == CCalType::sCAL_HOLOGRAPHY) {
        return CalTypeMod::CAL_HOLOGRAPHY;
    }
    	
    if (name == CCalType::sCAL_PHASE) {
        return CalTypeMod::CAL_PHASE;
    }
    	
    if (name == CCalType::sCAL_POINTING) {
        return CalTypeMod::CAL_POINTING;
    }
    	
    if (name == CCalType::sCAL_POINTING_MODEL) {
        return CalTypeMod::CAL_POINTING_MODEL;
    }
    	
    if (name == CCalType::sCAL_POSITION) {
        return CalTypeMod::CAL_POSITION;
    }
    	
    if (name == CCalType::sCAL_PRIMARY_BEAM) {
        return CalTypeMod::CAL_PRIMARY_BEAM;
    }
    	
    if (name == CCalType::sCAL_SEEING) {
        return CalTypeMod::CAL_SEEING;
    }
    	
    if (name == CCalType::sCAL_WVR) {
        return CalTypeMod::CAL_WVR;
    }
    
    throw badString(name);
}

CalTypeMod::CalType CCalType::from_int(unsigned int i) {
	vector<string> names = sCalTypeSet();
	if (i >= names.size()) throw badInt(i);
	return newCalType(names.at(i));
}

	

string CCalType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CalType'.";
}

string CCalType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CalType'.";
	return oss.str();
}

