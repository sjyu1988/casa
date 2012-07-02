
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
 * Warning!
 *  -------------------------------------------------------------------- 
 * | This is generated code!  Do not modify this file.                  |
 * | If you do, all changes will be lost when the file is re-generated. |
 *  --------------------------------------------------------------------
 *
 * File StateRow.cpp
 */
 
#include <vector>
using std::vector;

#include <set>
using std::set;

#include <ASDM.h>
#include <StateRow.h>
#include <StateTable.h>
	

using asdm::ASDM;
using asdm::StateRow;
using asdm::StateTable;


#include <Parser.h>
using asdm::Parser;

#include <EnumerationParser.h>
 
#include <InvalidArgumentException.h>
using asdm::InvalidArgumentException;

namespace asdm {

	StateRow::~StateRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	StateTable &StateRow::getTable() const {
		return table;
	}
	
	void StateRow::isAdded() {
		hasBeenAdded = true;
	}
	
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a StateRowIDL struct.
	 */
	StateRowIDL *StateRow::toIDL() const {
		StateRowIDL *x = new StateRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->stateId = stateId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->sig = sig;
 				
 			
		
	

	
  		
		
		
			
				
		x->ref = ref;
 				
 			
		
	

	
  		
		
		x->weightExists = weightExists;
		
		
			
				
		x->weight = weight;
 				
 			
		
	

	
  		
		
		
			
				
		x->onSky = onSky;
 				
 			
		
	

	
  		
		
		
			
				
		x->calDeviceName = calDeviceName;
 				
 			
		
	

	
  		
		
		
			
				
		x->flagRow = flagRow;
 				
 			
		
	

	
	
		
		
		return x;
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct StateRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void StateRow::setFromIDL (StateRowIDL x) throw(ConversionException) {
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setStateId(Tag (x.stateId));
			
 		
		
	

	
		
		
			
		setSig(x.sig);
  			
 		
		
	

	
		
		
			
		setRef(x.ref);
  			
 		
		
	

	
		
		weightExists = x.weightExists;
		if (x.weightExists) {
		
		
			
		setWeight(x.weight);
  			
 		
		
		}
		
	

	
		
		
			
		setOnSky(x.onSky);
  			
 		
		
	

	
		
		
			
		setCalDeviceName(x.calDeviceName);
  			
 		
		
	

	
		
		
			
		setFlagRow(x.flagRow);
  			
 		
		
	

	
	
		
		} catch (IllegalAccessException err) {
			throw new ConversionException (err.getMessage(),"State");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string StateRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(stateId, "stateId", buf);
		
		
	

  	
 		
		
		Parser::toXML(sig, "sig", buf);
		
		
	

  	
 		
		
		Parser::toXML(ref, "ref", buf);
		
		
	

  	
 		
		if (weightExists) {
		
		
		Parser::toXML(weight, "weight", buf);
		
		
		}
		
	

  	
 		
		
		Parser::toXML(onSky, "onSky", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("calDeviceName", calDeviceName));
		
		
	

  	
 		
		
		Parser::toXML(flagRow, "flagRow", buf);
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void StateRow::setFromXML (string rowDoc) throw(ConversionException) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setStateId(Parser::getTag("stateId","State",rowDoc));
			
		
	

	
  		
			
	  	setSig(Parser::getBoolean("sig","State",rowDoc));
			
		
	

	
  		
			
	  	setRef(Parser::getBoolean("ref","State",rowDoc));
			
		
	

	
  		
        if (row.isStr("<weight>")) {
			
	  		setWeight(Parser::getFloat("weight","State",rowDoc));
			
		}
 		
	

	
  		
			
	  	setOnSky(Parser::getBoolean("onSky","State",rowDoc));
			
		
	

	
		
		
		
		calDeviceName = EnumerationParser::getCalibrationDevice("calDeviceName","State",rowDoc);
		
		
		
	

	
  		
			
	  	setFlagRow(Parser::getBoolean("flagRow","State",rowDoc));
			
		
	

	
	
		
		} catch (IllegalAccessException err) {
			throw ConversionException (err.getMessage(),"State");
		}
	}
	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	

	
 	/**
 	 * Get stateId.
 	 * @return stateId as Tag
 	 */
 	Tag StateRow::getStateId() const {
	
  		return stateId;
 	}

 	/**
 	 * Set stateId with the specified Tag.
 	 * @param stateId The Tag value to which stateId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void StateRow::setStateId (Tag stateId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("stateId", "State");
		
  		}
  	
 		this->stateId = stateId;
	
 	}
	
	

	

	
 	/**
 	 * Get sig.
 	 * @return sig as bool
 	 */
 	bool StateRow::getSig() const {
	
  		return sig;
 	}

 	/**
 	 * Set sig with the specified bool.
 	 * @param sig The bool value to which sig is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setSig (bool sig)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->sig = sig;
	
 	}
	
	

	

	
 	/**
 	 * Get ref.
 	 * @return ref as bool
 	 */
 	bool StateRow::getRef() const {
	
  		return ref;
 	}

 	/**
 	 * Set ref with the specified bool.
 	 * @param ref The bool value to which ref is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setRef (bool ref)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->ref = ref;
	
 	}
	
	

	
	/**
	 * The attribute weight is optional. Return true if this attribute exists.
	 * @return true if and only if the weight attribute exists. 
	 */
	bool StateRow::isWeightExists() const {
		return weightExists;
	}
	

	
 	/**
 	 * Get weight, which is optional.
 	 * @return weight as float
 	 * @throw IllegalAccessException If weight does not exist.
 	 */
 	float StateRow::getWeight() const throw(IllegalAccessException) {
		if (!weightExists) {
			throw IllegalAccessException("weight", "State");
		}
	
  		return weight;
 	}

 	/**
 	 * Set weight with the specified float.
 	 * @param weight The float value to which weight is to be set.
 	 
 	
 	 */
 	void StateRow::setWeight (float weight) {
	
 		this->weight = weight;
	
		weightExists = true;
	
 	}
	
	
	/**
	 * Mark weight, which is an optional field, as non-existent.
	 */
	void StateRow::clearWeight () {
		weightExists = false;
	}
	

	

	
 	/**
 	 * Get onSky.
 	 * @return onSky as bool
 	 */
 	bool StateRow::getOnSky() const {
	
  		return onSky;
 	}

 	/**
 	 * Set onSky with the specified bool.
 	 * @param onSky The bool value to which onSky is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setOnSky (bool onSky)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->onSky = onSky;
	
 	}
	
	

	

	
 	/**
 	 * Get calDeviceName.
 	 * @return calDeviceName as CalibrationDeviceMod::CalibrationDevice
 	 */
 	CalibrationDeviceMod::CalibrationDevice StateRow::getCalDeviceName() const {
	
  		return calDeviceName;
 	}

 	/**
 	 * Set calDeviceName with the specified CalibrationDeviceMod::CalibrationDevice.
 	 * @param calDeviceName The CalibrationDeviceMod::CalibrationDevice value to which calDeviceName is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setCalDeviceName (CalibrationDeviceMod::CalibrationDevice calDeviceName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->calDeviceName = calDeviceName;
	
 	}
	
	

	

	
 	/**
 	 * Get flagRow.
 	 * @return flagRow as bool
 	 */
 	bool StateRow::getFlagRow() const {
	
  		return flagRow;
 	}

 	/**
 	 * Set flagRow with the specified bool.
 	 * @param flagRow The bool value to which flagRow is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setFlagRow (bool flagRow)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->flagRow = flagRow;
	
 	}
	
	

	
	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	/**
	 * Create a StateRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	StateRow::StateRow (StateTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	
		weightExists = false;
	

	

	

	

	
	
	
	
	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
calDeviceName = CCalibrationDevice::from_int(0);
	

	
	
	}
	
	StateRow::StateRow (StateTable &t, StateRow &row) : table(t) {
		hasBeenAdded = false;
		
		if (&row == 0) {
	
	
	

	

	

	
		weightExists = false;
	

	

	

	

			
		}
		else {
	
		
			stateId = row.stateId;
		
		
		
		
			sig = row.sig;
		
			ref = row.ref;
		
			onSky = row.onSky;
		
			calDeviceName = row.calDeviceName;
		
			flagRow = row.flagRow;
		
		
		
		
		if (row.weightExists) {
			weight = row.weight;		
			weightExists = true;
		}
		else
			weightExists = false;
		
		}	
	}

	
	bool StateRow::compareNoAutoInc(bool sig, bool ref, bool onSky, CalibrationDeviceMod::CalibrationDevice calDeviceName, bool flagRow) {
		bool result;
		result = true;
		
	
		
		result = result && (this->sig == sig);
		
		if (!result) return false;
	

	
		
		result = result && (this->ref == ref);
		
		if (!result) return false;
	

	
		
		result = result && (this->onSky == onSky);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDeviceName == calDeviceName);
		
		if (!result) return false;
	

	
		
		result = result && (this->flagRow == flagRow);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool StateRow::compareRequiredValue(bool sig, bool ref, bool onSky, CalibrationDeviceMod::CalibrationDevice calDeviceName, bool flagRow) {
		bool result;
		result = true;
		
	
		if (!(this->sig == sig)) return false;
	

	
		if (!(this->ref == ref)) return false;
	

	
		if (!(this->onSky == onSky)) return false;
	

	
		if (!(this->calDeviceName == calDeviceName)) return false;
	

	
		if (!(this->flagRow == flagRow)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the StateRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool StateRow::equalByRequiredValue(StateRow* x) {
		
			
		if (this->sig != x->sig) return false;
			
		if (this->ref != x->ref) return false;
			
		if (this->onSky != x->onSky) return false;
			
		if (this->calDeviceName != x->calDeviceName) return false;
			
		if (this->flagRow != x->flagRow) return false;
			
		
		return true;
	}	
	

} // End namespace asdm
 
