
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
 * File SysPowerRow.cpp
 */
 
#include <vector>
using std::vector;

#include <set>
using std::set;

#include <ASDM.h>
#include <SysPowerRow.h>
#include <SysPowerTable.h>

#include <AntennaTable.h>
#include <AntennaRow.h>

#include <SpectralWindowTable.h>
#include <SpectralWindowRow.h>

#include <FeedTable.h>
#include <FeedRow.h>
	

using asdm::ASDM;
using asdm::SysPowerRow;
using asdm::SysPowerTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::FeedTable;
using asdm::FeedRow;


#include <Parser.h>
using asdm::Parser;

#include <EnumerationParser.h>
 
#include <InvalidArgumentException.h>
using asdm::InvalidArgumentException;

namespace asdm {
	SysPowerRow::~SysPowerRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	SysPowerTable &SysPowerRow::getTable() const {
		return table;
	}

	bool SysPowerRow::isAdded() const {
		return hasBeenAdded;
	}	

	void SysPowerRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SysPowerRowIDL struct.
	 */
	SysPowerRowIDL *SysPowerRow::toIDL() const {
		SysPowerRowIDL *x = new SysPowerRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		x->switchedPowerDifferenceExists = switchedPowerDifferenceExists;
		
		
			
		x->switchedPowerDifference.length(switchedPowerDifference.size());
		for (unsigned int i = 0; i < switchedPowerDifference.size(); ++i) {
			
				
			x->switchedPowerDifference[i] = switchedPowerDifference.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->switchedPowerSumExists = switchedPowerSumExists;
		
		
			
		x->switchedPowerSum.length(switchedPowerSum.size());
		for (unsigned int i = 0; i < switchedPowerSum.size(); ++i) {
			
				
			x->switchedPowerSum[i] = switchedPowerSum.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->requantizerGainExists = requantizerGainExists;
		
		
			
		x->requantizerGain.length(requantizerGain.size());
		for (unsigned int i = 0; i < requantizerGain.size(); ++i) {
			
				
			x->requantizerGain[i] = requantizerGain.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x->feedId = feedId;
 				
 			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SysPowerRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void SysPowerRow::setFromIDL (SysPowerRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		switchedPowerDifferenceExists = x.switchedPowerDifferenceExists;
		if (x.switchedPowerDifferenceExists) {
		
		
			
		switchedPowerDifference .clear();
		for (unsigned int i = 0; i <x.switchedPowerDifference.length(); ++i) {
			
			switchedPowerDifference.push_back(x.switchedPowerDifference[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		switchedPowerSumExists = x.switchedPowerSumExists;
		if (x.switchedPowerSumExists) {
		
		
			
		switchedPowerSum .clear();
		for (unsigned int i = 0; i <x.switchedPowerSum.length(); ++i) {
			
			switchedPowerSum.push_back(x.switchedPowerSum[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		requantizerGainExists = x.requantizerGainExists;
		if (x.requantizerGainExists) {
		
		
			
		requantizerGain .clear();
		for (unsigned int i = 0; i <x.requantizerGain.length(); ++i) {
			
			requantizerGain.push_back(x.requantizerGain[i]);
  			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
		
			
		setFeedId(x.feedId);
  			
 		
		
	

	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		
	

	

	

		} catch (IllegalAccessException err) {
			throw ConversionException (err.getMessage(),"SysPower");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string SysPowerRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		if (switchedPowerDifferenceExists) {
		
		
		Parser::toXML(switchedPowerDifference, "switchedPowerDifference", buf);
		
		
		}
		
	

  	
 		
		if (switchedPowerSumExists) {
		
		
		Parser::toXML(switchedPowerSum, "switchedPowerSum", buf);
		
		
		}
		
	

  	
 		
		if (requantizerGainExists) {
		
		
		Parser::toXML(requantizerGain, "requantizerGain", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(feedId, "feedId", buf);
		
		
	

  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void SysPowerRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","SysPower",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","SysPower",rowDoc));
			
		
	

	
  		
        if (row.isStr("<switchedPowerDifference>")) {
			
								
	  		setSwitchedPowerDifference(Parser::get1DFloat("switchedPowerDifference","SysPower",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<switchedPowerSum>")) {
			
								
	  		setSwitchedPowerSum(Parser::get1DFloat("switchedPowerSum","SysPower",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<requantizerGain>")) {
			
								
	  		setRequantizerGain(Parser::get1DFloat("requantizerGain","SysPower",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
  		
			
	  	setFeedId(Parser::getInteger("feedId","Feed",rowDoc));
			
		
	

	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","SpectralWindow",rowDoc));
			
		
	

	
		
	

	

	

		} catch (IllegalAccessException err) {
			throw ConversionException (err.getMessage(),"SysPower");
		}
	}
	
	void SysPowerRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
	spectralWindowId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(feedId);
				
		
	

	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	


	
	
	eoss.writeBoolean(switchedPowerDifferenceExists);
	if (switchedPowerDifferenceExists) {
	
	
	
		
		
			
		eoss.writeInt((int) switchedPowerDifference.size());
		for (unsigned int i = 0; i < switchedPowerDifference.size(); i++)
				
			eoss.writeFloat(switchedPowerDifference.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(switchedPowerSumExists);
	if (switchedPowerSumExists) {
	
	
	
		
		
			
		eoss.writeInt((int) switchedPowerSum.size());
		for (unsigned int i = 0; i < switchedPowerSum.size(); i++)
				
			eoss.writeFloat(switchedPowerSum.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(requantizerGainExists);
	if (requantizerGainExists) {
	
	
	
		
		
			
		eoss.writeInt((int) requantizerGain.size());
		for (unsigned int i = 0; i < requantizerGain.size(); i++)
				
			eoss.writeFloat(requantizerGain.at(i));
				
				
						
		
	

	}

	}
	
void SysPowerRow::antennaIdFromBin(EndianISStream& eiss) {
		
	
		
		
		antennaId =  Tag::fromBin(eiss);
		
	
	
}
void SysPowerRow::spectralWindowIdFromBin(EndianISStream& eiss) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eiss);
		
	
	
}
void SysPowerRow::feedIdFromBin(EndianISStream& eiss) {
		
	
	
		
			
		feedId =  eiss.readInt();
			
		
	
	
}
void SysPowerRow::timeIntervalFromBin(EndianISStream& eiss) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eiss);
		
	
	
}
void SysPowerRow::numReceptorFromBin(EndianISStream& eiss) {
		
	
	
		
			
		numReceptor =  eiss.readInt();
			
		
	
	
}

void SysPowerRow::switchedPowerDifferenceFromBin(EndianISStream& eiss) {
		
	switchedPowerDifferenceExists = eiss.readBoolean();
	if (switchedPowerDifferenceExists) {
		
	
	
		
			
	
		switchedPowerDifference.clear();
		
		unsigned int switchedPowerDifferenceDim1 = eiss.readInt();
		for (unsigned int  i = 0 ; i < switchedPowerDifferenceDim1; i++)
			
			switchedPowerDifference.push_back(eiss.readFloat());
			
	

		
	

	}
	
}
void SysPowerRow::switchedPowerSumFromBin(EndianISStream& eiss) {
		
	switchedPowerSumExists = eiss.readBoolean();
	if (switchedPowerSumExists) {
		
	
	
		
			
	
		switchedPowerSum.clear();
		
		unsigned int switchedPowerSumDim1 = eiss.readInt();
		for (unsigned int  i = 0 ; i < switchedPowerSumDim1; i++)
			
			switchedPowerSum.push_back(eiss.readFloat());
			
	

		
	

	}
	
}
void SysPowerRow::requantizerGainFromBin(EndianISStream& eiss) {
		
	requantizerGainExists = eiss.readBoolean();
	if (requantizerGainExists) {
		
	
	
		
			
	
		requantizerGain.clear();
		
		unsigned int requantizerGainDim1 = eiss.readInt();
		for (unsigned int  i = 0 ; i < requantizerGainDim1; i++)
			
			requantizerGain.push_back(eiss.readFloat());
			
	

		
	

	}
	
}
	
	
	SysPowerRow* SysPowerRow::fromBin(EndianISStream& eiss, SysPowerTable& table, const vector<string>& attributesSeq) {
		SysPowerRow* row = new  SysPowerRow(table);
		
		map<string, SysPowerAttributeFromBin>::iterator iter ;
		for (unsigned int i = 0; i < attributesSeq.size(); i++) {
			iter = row->fromBinMethods.find(attributesSeq.at(i));
			if (iter != row->fromBinMethods.end()) {
				(row->*(row->fromBinMethods[ attributesSeq.at(i) ] ))(eiss);			
			}
			else {
				BinaryAttributeReaderFunctor* functorP = table.getUnknownAttributeBinaryReader(attributesSeq.at(i));
				if (functorP)
					(*functorP)(eiss);
				else
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "SysPowerTable");
			}
				
		}				
		return row;
	}
	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval SysPowerRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SysPowerRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "SysPower");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int SysPowerRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void SysPowerRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	
	/**
	 * The attribute switchedPowerDifference is optional. Return true if this attribute exists.
	 * @return true if and only if the switchedPowerDifference attribute exists. 
	 */
	bool SysPowerRow::isSwitchedPowerDifferenceExists() const {
		return switchedPowerDifferenceExists;
	}
	

	
 	/**
 	 * Get switchedPowerDifference, which is optional.
 	 * @return switchedPowerDifference as vector<float >
 	 * @throw IllegalAccessException If switchedPowerDifference does not exist.
 	 */
 	vector<float > SysPowerRow::getSwitchedPowerDifference() const  {
		if (!switchedPowerDifferenceExists) {
			throw IllegalAccessException("switchedPowerDifference", "SysPower");
		}
	
  		return switchedPowerDifference;
 	}

 	/**
 	 * Set switchedPowerDifference with the specified vector<float >.
 	 * @param switchedPowerDifference The vector<float > value to which switchedPowerDifference is to be set.
 	 
 	
 	 */
 	void SysPowerRow::setSwitchedPowerDifference (vector<float > switchedPowerDifference) {
	
 		this->switchedPowerDifference = switchedPowerDifference;
	
		switchedPowerDifferenceExists = true;
	
 	}
	
	
	/**
	 * Mark switchedPowerDifference, which is an optional field, as non-existent.
	 */
	void SysPowerRow::clearSwitchedPowerDifference () {
		switchedPowerDifferenceExists = false;
	}
	

	
	/**
	 * The attribute switchedPowerSum is optional. Return true if this attribute exists.
	 * @return true if and only if the switchedPowerSum attribute exists. 
	 */
	bool SysPowerRow::isSwitchedPowerSumExists() const {
		return switchedPowerSumExists;
	}
	

	
 	/**
 	 * Get switchedPowerSum, which is optional.
 	 * @return switchedPowerSum as vector<float >
 	 * @throw IllegalAccessException If switchedPowerSum does not exist.
 	 */
 	vector<float > SysPowerRow::getSwitchedPowerSum() const  {
		if (!switchedPowerSumExists) {
			throw IllegalAccessException("switchedPowerSum", "SysPower");
		}
	
  		return switchedPowerSum;
 	}

 	/**
 	 * Set switchedPowerSum with the specified vector<float >.
 	 * @param switchedPowerSum The vector<float > value to which switchedPowerSum is to be set.
 	 
 	
 	 */
 	void SysPowerRow::setSwitchedPowerSum (vector<float > switchedPowerSum) {
	
 		this->switchedPowerSum = switchedPowerSum;
	
		switchedPowerSumExists = true;
	
 	}
	
	
	/**
	 * Mark switchedPowerSum, which is an optional field, as non-existent.
	 */
	void SysPowerRow::clearSwitchedPowerSum () {
		switchedPowerSumExists = false;
	}
	

	
	/**
	 * The attribute requantizerGain is optional. Return true if this attribute exists.
	 * @return true if and only if the requantizerGain attribute exists. 
	 */
	bool SysPowerRow::isRequantizerGainExists() const {
		return requantizerGainExists;
	}
	

	
 	/**
 	 * Get requantizerGain, which is optional.
 	 * @return requantizerGain as vector<float >
 	 * @throw IllegalAccessException If requantizerGain does not exist.
 	 */
 	vector<float > SysPowerRow::getRequantizerGain() const  {
		if (!requantizerGainExists) {
			throw IllegalAccessException("requantizerGain", "SysPower");
		}
	
  		return requantizerGain;
 	}

 	/**
 	 * Set requantizerGain with the specified vector<float >.
 	 * @param requantizerGain The vector<float > value to which requantizerGain is to be set.
 	 
 	
 	 */
 	void SysPowerRow::setRequantizerGain (vector<float > requantizerGain) {
	
 		this->requantizerGain = requantizerGain;
	
		requantizerGainExists = true;
	
 	}
	
	
	/**
	 * Mark requantizerGain, which is an optional field, as non-existent.
	 */
	void SysPowerRow::clearRequantizerGain () {
		requantizerGainExists = false;
	}
	

	
	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag SysPowerRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SysPowerRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "SysPower");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get feedId.
 	 * @return feedId as int
 	 */
 	int SysPowerRow::getFeedId() const {
	
  		return feedId;
 	}

 	/**
 	 * Set feedId with the specified int.
 	 * @param feedId The int value to which feedId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SysPowerRow::setFeedId (int feedId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("feedId", "SysPower");
		
  		}
  	
 		this->feedId = feedId;
	
 	}
	
	

	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag SysPowerRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SysPowerRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("spectralWindowId", "SysPower");
		
  		}
  	
 		this->spectralWindowId = spectralWindowId;
	
 	}
	
	

	///////////
	// Links //
	///////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* SysPowerRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* SysPowerRow::getSpectralWindowUsingSpectralWindowId() {
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId);
	 }
	 

	

	
	
	
		

	// ===> Slice link from a row of SysPower table to a collection of row of Feed table.
	
	/**
	 * Get the collection of row in the Feed table having their attribut feedId == this->feedId
	 */
	vector <FeedRow *> SysPowerRow::getFeeds() {
		
			return table.getContainer().getFeed().getRowByFeedId(feedId);
		
	}
	

	

	
	/**
	 * Create a SysPowerRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SysPowerRow::SysPowerRow (SysPowerTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	
		switchedPowerDifferenceExists = false;
	

	
		switchedPowerSumExists = false;
	

	
		requantizerGainExists = false;
	

	
	

	

	

	
	
	
	

	

	

	

	

	
	
	 fromBinMethods["antennaId"] = &SysPowerRow::antennaIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &SysPowerRow::spectralWindowIdFromBin; 
	 fromBinMethods["feedId"] = &SysPowerRow::feedIdFromBin; 
	 fromBinMethods["timeInterval"] = &SysPowerRow::timeIntervalFromBin; 
	 fromBinMethods["numReceptor"] = &SysPowerRow::numReceptorFromBin; 
		
	
	 fromBinMethods["switchedPowerDifference"] = &SysPowerRow::switchedPowerDifferenceFromBin; 
	 fromBinMethods["switchedPowerSum"] = &SysPowerRow::switchedPowerSumFromBin; 
	 fromBinMethods["requantizerGain"] = &SysPowerRow::requantizerGainFromBin; 
	
	}
	
	SysPowerRow::SysPowerRow (SysPowerTable &t, SysPowerRow &row) : table(t) {
		hasBeenAdded = false;
		
		if (&row == 0) {
	
	
	

	

	
		switchedPowerDifferenceExists = false;
	

	
		switchedPowerSumExists = false;
	

	
		requantizerGainExists = false;
	

	
	

	

	
		
		}
		else {
	
		
			antennaId = row.antennaId;
		
			spectralWindowId = row.spectralWindowId;
		
			feedId = row.feedId;
		
			timeInterval = row.timeInterval;
		
		
		
		
			numReceptor = row.numReceptor;
		
		
		
		
		if (row.switchedPowerDifferenceExists) {
			switchedPowerDifference = row.switchedPowerDifference;		
			switchedPowerDifferenceExists = true;
		}
		else
			switchedPowerDifferenceExists = false;
		
		if (row.switchedPowerSumExists) {
			switchedPowerSum = row.switchedPowerSum;		
			switchedPowerSumExists = true;
		}
		else
			switchedPowerSumExists = false;
		
		if (row.requantizerGainExists) {
			requantizerGain = row.requantizerGain;		
			requantizerGainExists = true;
		}
		else
			requantizerGainExists = false;
		
		}
		
		 fromBinMethods["antennaId"] = &SysPowerRow::antennaIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &SysPowerRow::spectralWindowIdFromBin; 
		 fromBinMethods["feedId"] = &SysPowerRow::feedIdFromBin; 
		 fromBinMethods["timeInterval"] = &SysPowerRow::timeIntervalFromBin; 
		 fromBinMethods["numReceptor"] = &SysPowerRow::numReceptorFromBin; 
			
	
		 fromBinMethods["switchedPowerDifference"] = &SysPowerRow::switchedPowerDifferenceFromBin; 
		 fromBinMethods["switchedPowerSum"] = &SysPowerRow::switchedPowerSumFromBin; 
		 fromBinMethods["requantizerGain"] = &SysPowerRow::requantizerGainFromBin; 
			
	}

	
	bool SysPowerRow::compareNoAutoInc(Tag antennaId, Tag spectralWindowId, int feedId, ArrayTimeInterval timeInterval, int numReceptor) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->spectralWindowId == spectralWindowId);
		
		if (!result) return false;
	

	
		
		result = result && (this->feedId == feedId);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool SysPowerRow::compareRequiredValue(int numReceptor) {
		bool result;
		result = true;
		
	
		if (!(this->numReceptor == numReceptor)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the SysPowerRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool SysPowerRow::equalByRequiredValue(SysPowerRow* x) {
		
			
		if (this->numReceptor != x->numReceptor) return false;
			
		
		return true;
	}	
	
/*
	 map<string, SysPowerAttributeFromBin> SysPowerRow::initFromBinMethods() {
		map<string, SysPowerAttributeFromBin> result;
		
		result["antennaId"] = &SysPowerRow::antennaIdFromBin;
		result["spectralWindowId"] = &SysPowerRow::spectralWindowIdFromBin;
		result["feedId"] = &SysPowerRow::feedIdFromBin;
		result["timeInterval"] = &SysPowerRow::timeIntervalFromBin;
		result["numReceptor"] = &SysPowerRow::numReceptorFromBin;
		
		
		result["switchedPowerDifference"] = &SysPowerRow::switchedPowerDifferenceFromBin;
		result["switchedPowerSum"] = &SysPowerRow::switchedPowerSumFromBin;
		result["requantizerGain"] = &SysPowerRow::requantizerGainFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
