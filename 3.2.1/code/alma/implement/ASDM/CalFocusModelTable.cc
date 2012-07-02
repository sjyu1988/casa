
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
 * File CalFocusModelTable.cpp
 */
#include <ConversionException.h>
#include <DuplicateKey.h>
#include <OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <ASDM.h>
#include <CalFocusModelTable.h>
#include <CalFocusModelRow.h>
#include <Parser.h>

using asdm::ASDM;
using asdm::CalFocusModelTable;
using asdm::CalFocusModelRow;
using asdm::Parser;

#include <iostream>
#include <sstream>
#include <set>
using namespace std;

#include <Misc.h>
using namespace asdm;

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "boost/filesystem/operations.hpp"


namespace asdm {

	string CalFocusModelTable::tableName = "CalFocusModel";
	const vector<string> CalFocusModelTable::attributesNames = initAttributesNames();
		

	/**
	 * The list of field names that make up key key.
	 * (Initialization is in the constructor.)
	 */
	vector<string> CalFocusModelTable::key;

	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 */	
	vector<string> CalFocusModelTable::getKeyName() {
		return key;
	}


	CalFocusModelTable::CalFocusModelTable(ASDM &c) : container(c) {

	
		key.push_back("antennaName");
	
		key.push_back("receiverBand");
	
		key.push_back("polarizationType");
	
		key.push_back("calDataId");
	
		key.push_back("calReductionId");
	


		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("CalFocusModelTable");
		entity.setEntityVersion("1");
		entity.setInstanceVersion("1");
		
		// Archive XML
		archiveAsBin = false;
		
		// File XML
		fileAsBin = false;
		
		// By default the table is considered as present in memory
		presentInMemory = true;
		
		// By default there is no load in progress
		loadInProgress = false;
	}
	
/**
 * A destructor for CalFocusModelTable.
 */
	CalFocusModelTable::~CalFocusModelTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &CalFocusModelTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int CalFocusModelTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string CalFocusModelTable::getName() const {
		return tableName;
	}
	
	/**
	 * Build the vector of attributes names.
	 */
	vector<string> CalFocusModelTable::initAttributesNames() {
		vector<string> attributesNames;

		attributesNames.push_back("antennaName");

		attributesNames.push_back("receiverBand");

		attributesNames.push_back("polarizationType");

		attributesNames.push_back("calDataId");

		attributesNames.push_back("calReductionId");


		attributesNames.push_back("startValidTime");

		attributesNames.push_back("endValidTime");

		attributesNames.push_back("antennaMake");

		attributesNames.push_back("numCoeff");

		attributesNames.push_back("numSourceObs");

		attributesNames.push_back("coeffName");

		attributesNames.push_back("coeffFormula");

		attributesNames.push_back("coeffValue");

		attributesNames.push_back("coeffError");

		attributesNames.push_back("coeffFixed");

		attributesNames.push_back("focusModel");

		attributesNames.push_back("focusRMS");

		attributesNames.push_back("reducedChiSquared");


		return attributesNames;
	}
	
	/**
	 * Return the names of the attributes.
	 */
	const vector<string>& CalFocusModelTable::getAttributesNames() { return attributesNames; }

	/**
	 * Return this table's Entity.
	 */
	Entity CalFocusModelTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void CalFocusModelTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	CalFocusModelRow *CalFocusModelTable::newRow() {
		return new CalFocusModelRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName 
	
 	 * @param receiverBand 
	
 	 * @param polarizationType 
	
 	 * @param calDataId 
	
 	 * @param calReductionId 
	
 	 * @param startValidTime 
	
 	 * @param endValidTime 
	
 	 * @param antennaMake 
	
 	 * @param numCoeff 
	
 	 * @param numSourceObs 
	
 	 * @param coeffName 
	
 	 * @param coeffFormula 
	
 	 * @param coeffValue 
	
 	 * @param coeffError 
	
 	 * @param coeffFixed 
	
 	 * @param focusModel 
	
 	 * @param focusRMS 
	
 	 * @param reducedChiSquared 
	
     */
	CalFocusModelRow* CalFocusModelTable::newRow(string antennaName, ReceiverBandMod::ReceiverBand receiverBand, PolarizationTypeMod::PolarizationType polarizationType, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numCoeff, int numSourceObs, vector<string > coeffName, vector<string > coeffFormula, vector<float > coeffValue, vector<float > coeffError, vector<bool > coeffFixed, string focusModel, vector<Length > focusRMS, double reducedChiSquared){
		CalFocusModelRow *row = new CalFocusModelRow(*this);
			
		row->setAntennaName(antennaName);
			
		row->setReceiverBand(receiverBand);
			
		row->setPolarizationType(polarizationType);
			
		row->setCalDataId(calDataId);
			
		row->setCalReductionId(calReductionId);
			
		row->setStartValidTime(startValidTime);
			
		row->setEndValidTime(endValidTime);
			
		row->setAntennaMake(antennaMake);
			
		row->setNumCoeff(numCoeff);
			
		row->setNumSourceObs(numSourceObs);
			
		row->setCoeffName(coeffName);
			
		row->setCoeffFormula(coeffFormula);
			
		row->setCoeffValue(coeffValue);
			
		row->setCoeffError(coeffError);
			
		row->setCoeffFixed(coeffFixed);
			
		row->setFocusModel(focusModel);
			
		row->setFocusRMS(focusRMS);
			
		row->setReducedChiSquared(reducedChiSquared);
	
		return row;		
	}	
	


CalFocusModelRow* CalFocusModelTable::newRow(CalFocusModelRow* row) {
	return new CalFocusModelRow(*this, *row);
}

	//
	// Append a row to its table.
	//

	
	 
	/**
	 * Add a row.
	 * @throws DuplicateKey Thrown if the new row has a key that is already in the table.
	 * @param x A pointer to the row to be added.
	 * @return x
	 */
	CalFocusModelRow* CalFocusModelTable::add(CalFocusModelRow* x) {
		
		if (getRowByKey(
						x->getAntennaName()
						,
						x->getReceiverBand()
						,
						x->getPolarizationType()
						,
						x->getCalDataId()
						,
						x->getCalReductionId()
						))
			//throw DuplicateKey(x.getAntennaName() + "|" + x.getReceiverBand() + "|" + x.getPolarizationType() + "|" + x.getCalDataId() + "|" + x.getCalReductionId(),"CalFocusModel");
			throw DuplicateKey("Duplicate key exception in ","CalFocusModelTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}

		





	// 
	// A private method to append a row to its table, used by input conversion
	// methods.
	//

	
	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @param x a pointer on the row to be appended.
	 * @returns a pointer on x.
	 * @throws DuplicateKey
	 
	 */
	CalFocusModelRow*  CalFocusModelTable::checkAndAdd(CalFocusModelRow* x)  {
		
		
		if (getRowByKey(
	
			x->getAntennaName()
	,
			x->getReceiverBand()
	,
			x->getPolarizationType()
	,
			x->getCalDataId()
	,
			x->getCalReductionId()
			
		)) throw DuplicateKey("Duplicate key exception in ", "CalFocusModelTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	






	 vector<CalFocusModelRow *> CalFocusModelTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<CalFocusModelRow *>& CalFocusModelTable::get() const {
	 	const_cast<CalFocusModelTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a CalFocusModelRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	CalFocusModelRow* CalFocusModelTable::getRowByKey(string antennaName, ReceiverBandMod::ReceiverBand receiverBand, PolarizationTypeMod::PolarizationType polarizationType, Tag calDataId, Tag calReductionId)  {
 	checkPresenceInMemory();
	CalFocusModelRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->antennaName != antennaName) continue;
			
		
			
				if (aRow->receiverBand != receiverBand) continue;
			
		
			
				if (aRow->polarizationType != polarizationType) continue;
			
		
			
				if (aRow->calDataId != calDataId) continue;
			
		
			
				if (aRow->calReductionId != calReductionId) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param antennaName.
 	 		
 * @param receiverBand.
 	 		
 * @param polarizationType.
 	 		
 * @param calDataId.
 	 		
 * @param calReductionId.
 	 		
 * @param startValidTime.
 	 		
 * @param endValidTime.
 	 		
 * @param antennaMake.
 	 		
 * @param numCoeff.
 	 		
 * @param numSourceObs.
 	 		
 * @param coeffName.
 	 		
 * @param coeffFormula.
 	 		
 * @param coeffValue.
 	 		
 * @param coeffError.
 	 		
 * @param coeffFixed.
 	 		
 * @param focusModel.
 	 		
 * @param focusRMS.
 	 		
 * @param reducedChiSquared.
 	 		 
 */
CalFocusModelRow* CalFocusModelTable::lookup(string antennaName, ReceiverBandMod::ReceiverBand receiverBand, PolarizationTypeMod::PolarizationType polarizationType, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numCoeff, int numSourceObs, vector<string > coeffName, vector<string > coeffFormula, vector<float > coeffValue, vector<float > coeffError, vector<bool > coeffFixed, string focusModel, vector<Length > focusRMS, double reducedChiSquared) {
		CalFocusModelRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(antennaName, receiverBand, polarizationType, calDataId, calReductionId, startValidTime, endValidTime, antennaMake, numCoeff, numSourceObs, coeffName, coeffFormula, coeffValue, coeffError, coeffFixed, focusModel, focusRMS, reducedChiSquared)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	




#ifndef WITHOUT_ACS
	// Conversion Methods

	CalFocusModelTableIDL *CalFocusModelTable::toIDL() {
		CalFocusModelTableIDL *x = new CalFocusModelTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<CalFocusModelRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			x->row[i] = *(v[i]->toIDL());
		}
		return x;
	}
#endif
	
#ifndef WITHOUT_ACS
	void CalFocusModelTable::fromIDL(CalFocusModelTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			CalFocusModelRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}
#endif

	
	string CalFocusModelTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<CalFocusModelTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:clfcsm=\"http://Alma/XASDM/CalFocusModelTable\" xsi:schemaLocation=\"http://Alma/XASDM/CalFocusModelTable http://almaobservatory.org/XML/XASDM/2/CalFocusModelTable.xsd\" schemaVersion=\"2\" schemaRevision=\"1.58\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<CalFocusModelRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (NoSuchRow e) {
			}
			buf.append("  ");
		}		
		buf.append("</CalFocusModelTable> ");
		return buf;
	}

	
	void CalFocusModelTable::fromXML(string& xmlDoc)  {
		Parser xml(xmlDoc);
		if (!xml.isStr("<CalFocusModelTable")) 
			error();
		// cout << "Parsing a CalFocusModelTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "CalFocusModelTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		CalFocusModelRow *row;
		while (s.length() != 0) {
			row = newRow();
			row->setFromXML(s);
			try {
				checkAndAdd(row);
			} catch (DuplicateKey e1) {
				throw ConversionException(e1.getMessage(),"CalFocusModelTable");
			} 
			catch (UniquenessViolationException e1) {
				throw ConversionException(e1.getMessage(),"CalFocusModelTable");	
			}
			catch (...) {
				// cout << "Unexpected error in CalFocusModelTable::checkAndAdd called from CalFocusModelTable::fromXML " << endl;
			}
			s = xml.getElementContent("<row>","</row>");
		}
		if (!xml.isStr("</CalFocusModelTable>")) 
			error();
			
		archiveAsBin = false;
		fileAsBin = false;
		
	}

	
	void CalFocusModelTable::error()  {
		throw ConversionException("Invalid xml document","CalFocusModel");
	}
	
	
	string CalFocusModelTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<CalFocusModelTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:clfcsm=\"http://Alma/XASDM/CalFocusModelTable\" xsi:schemaLocation=\"http://Alma/XASDM/CalFocusModelTable http://almaobservatory.org/XML/XASDM/2/CalFocusModelTable.xsd\" schemaVersion=\"2\" schemaRevision=\"1.58\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='CalFocusModelTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<antennaName/>\n"; 
		oss << "<receiverBand/>\n"; 
		oss << "<polarizationType/>\n"; 
		oss << "<calDataId/>\n"; 
		oss << "<calReductionId/>\n"; 
		oss << "<startValidTime/>\n"; 
		oss << "<endValidTime/>\n"; 
		oss << "<antennaMake/>\n"; 
		oss << "<numCoeff/>\n"; 
		oss << "<numSourceObs/>\n"; 
		oss << "<coeffName/>\n"; 
		oss << "<coeffFormula/>\n"; 
		oss << "<coeffValue/>\n"; 
		oss << "<coeffError/>\n"; 
		oss << "<coeffFixed/>\n"; 
		oss << "<focusModel/>\n"; 
		oss << "<focusRMS/>\n"; 
		oss << "<reducedChiSquared/>\n"; 

		oss << "</Attributes>\n";		
		oss << "</CalFocusModelTable>\n";

		return oss.str();				
	}
	
	string CalFocusModelTable::toMIME(const asdm::ByteOrder* byteOrder) {
		EndianOSStream eoss(byteOrder);
		
		string UID = getEntity().getEntityId().toString();
		
		// The MIME Header
		eoss <<"MIME-Version: 1.0";
		eoss << "\n";
		eoss << "Content-Type: Multipart/Related; boundary='MIME_boundary'; type='text/xml'; start= '<header.xml>'";
		eoss <<"\n";
		eoss <<"Content-Description: Correlator";
		eoss <<"\n";
		eoss <<"alma-uid:" << UID;
		eoss <<"\n";
		eoss <<"\n";		
		
		// The MIME XML part header.
		eoss <<"--MIME_boundary";
		eoss <<"\n";
		eoss <<"Content-Type: text/xml; charset='ISO-8859-1'";
		eoss <<"\n";
		eoss <<"Content-Transfer-Encoding: 8bit";
		eoss <<"\n";
		eoss <<"Content-ID: <header.xml>";
		eoss <<"\n";
		eoss <<"\n";
		
		// The MIME XML part content.
		eoss << MIMEXMLPart(byteOrder);

		// The MIME binary part header
		eoss <<"--MIME_boundary";
		eoss <<"\n";
		eoss <<"Content-Type: binary/octet-stream";
		eoss <<"\n";
		eoss <<"Content-ID: <content.bin>";
		eoss <<"\n";
		eoss <<"\n";	
		
		// The MIME binary content
		entity.toBin(eoss);
		container.getEntity().toBin(eoss);
		eoss.writeInt((int) privateRows.size());
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			privateRows.at(i)->toBin(eoss);	
		}
		
		// The closing MIME boundary
		eoss << "\n--MIME_boundary--";
		eoss << "\n";
		
		return eoss.str();	
	}

	
	void CalFocusModelTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "CalFocusModel");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "CalFocusModel");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalFocusModel");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalFocusModel");
    
    const ByteOrder* byteOrder;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
     
    attributesSeq.push_back("antennaName") ; 
     
    attributesSeq.push_back("receiverBand") ; 
     
    attributesSeq.push_back("polarizationType") ; 
     
    attributesSeq.push_back("calDataId") ; 
     
    attributesSeq.push_back("calReductionId") ; 
     
    attributesSeq.push_back("startValidTime") ; 
     
    attributesSeq.push_back("endValidTime") ; 
     
    attributesSeq.push_back("antennaMake") ; 
     
    attributesSeq.push_back("numCoeff") ; 
     
    attributesSeq.push_back("numSourceObs") ; 
     
    attributesSeq.push_back("coeffName") ; 
     
    attributesSeq.push_back("coeffFormula") ; 
     
    attributesSeq.push_back("coeffValue") ; 
     
    attributesSeq.push_back("coeffError") ; 
     
    attributesSeq.push_back("coeffFixed") ; 
     
    attributesSeq.push_back("focusModel") ; 
     
    attributesSeq.push_back("focusRMS") ; 
     
    attributesSeq.push_back("reducedChiSquared") ; 
    
              
     }
    else if (string("CalFocusModelTable").compare((const char*) root_element->name) == 0) {
      // It's a new (and correct) MIME file for tables.
      //
      // 1st )  Look for a BulkStoreRef element with an attribute byteOrder.
      //
      xmlNode* bulkStoreRef = 0;
      xmlNode* child = root_element->children;
      
      // Skip the two first children (Entity and ContainerEntity).
      bulkStoreRef = (child ==  0) ? 0 : ( (child->next) == 0 ? 0 : child->next->next );
      
      if ( bulkStoreRef == 0 || (bulkStoreRef->type != XML_ELEMENT_NODE)  || (string("BulkStoreRef").compare((const char*) bulkStoreRef->name) != 0))
      	throw ConversionException ("Could not find the element '/CalFocusModelTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "CalFocusModel");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/CalFocusModelTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "CalFocusModel");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/CalFocusModelTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "CalFocusModel");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/CalFocusModelTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "CalFocusModel");
 
 	xmlNode* childOfAttributes = attributes->children;
 	
 	while ( childOfAttributes != 0 && (childOfAttributes->type == XML_ELEMENT_NODE) ) {
 		attributesSeq.push_back(string((const char*) childOfAttributes->name));
 		childOfAttributes = childOfAttributes->next;
    }
    }
    // Create an EndianISStream from the substring containing the binary part.
    EndianISStream eiss(mimeMsg.substr(loc1+binPartMIMEHeader.size()), byteOrder);
    
    entity = Entity::fromBin(eiss);
    
    // We do nothing with that but we have to read it.
    Entity containerEntity = Entity::fromBin(eiss);

	// Let's read numRows but ignore it and rely on the value specified in the ASDM.xml file.    
    int numRows = eiss.readInt();
    if ((numRows != -1)                        // Then these are *not* data produced at the EVLA.
    	&& ((unsigned int) numRows != this->declaredSize )) { // Then the declared size (in ASDM.xml) is not equal to the one 
    	                                       // written into the binary representation of the table.
		cout << "The a number of rows ('" 
			 << numRows
			 << "') declared in the binary representation of the table is different from the one declared in ASDM.xml ('"
			 << this->declaredSize
			 << "'). I'll proceed with the value declared in ASDM.xml"
			 << endl;
    }                                           

    try {
      for (uint32_t i = 0; i < this->declaredSize; i++) {
	CalFocusModelRow* aRow = CalFocusModelRow::fromBin(eiss, *this, attributesSeq);
	checkAndAdd(aRow);
      }
    }
    catch (DuplicateKey e) {
      throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "CalFocusModel");
    }
    catch (TagFormatException e) {
      throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "CalFocusModel");
    }
    archiveAsBin = true;
    fileAsBin = true;
	}

	
	void CalFocusModelTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/CalFocusModel.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "CalFocusModel");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "CalFocusModel");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/CalFocusModel.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "CalFocusModel");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "CalFocusModel");
		}
	}

	
	void CalFocusModelTable::setFromFile(const string& directory) {		
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/CalFocusModel.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/CalFocusModel.bin"))))
      setFromMIMEFile(directory);
    else
      throw ConversionException("No file found for the CalFocusModel table", "CalFocusModel");
	}			

	
  void CalFocusModelTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/CalFocusModel.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "CalFocusModel");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"CalFocusModel");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"CalFocusModel");
    
    setFromMIME(ss.str());
  }	

	
void CalFocusModelTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/CalFocusModel.xml";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "CalFocusModel");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "CalFocusModel");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "CalFocusModel");

    // Let's make a string out of the stringstream content and empty the stringstream.
    string xmlDocument = ss.str(); ss.str("");

    // Let's make a very primitive check to decide
    // whether the XML content represents the table
    // or refers to it via a <BulkStoreRef element.
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
} // End namespace asdm
 
