//# CTInterface.cc: Implementation of CTInterface class
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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
//----------------------------------------------------------------------------

#include <synthesis/CalTables/CTInterface.h>
#include <ms/MeasurementSets/MSColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  //
  //----------------------------------------------------------------------------
  //
  CTInterface::CTInterface(const Table& table)
    :MSSelectableTable(table), fakeDDSubTable()
  {makeDDSubTable();};
  //
  //----------------------------------------------------------------------------
  //
  CTInterface::~CTInterface() 
  {};
  //
  //----------------------------------------------------------------------------
  //
  const NewCalTable* CTInterface::asCT()
  {
    return static_cast<const NewCalTable *>(table());
  };
  //
  //----------------------------------------------------------------------------
  //
  void CTInterface::makeDDSubTable()
  {
    //
    // Make an in-memory DataDescription table.
    //
    SetupNewTable setup(String(""), MSDataDescription::requiredTableDesc(), 
			Table::Scratch);
    const Table tmpTab(setup, Table::Memory, spectralWindow().nrow());
    fakeDDSubTable = MSDataDescription(tmpTab);
    //
    // Call the in-memory table what it is.
    //
    TableInfo& tabInfo=fakeDDSubTable.tableInfo();
    tabInfo.setType("FAKE_DATA_DESCRIPTION_ID");
    //
    // Fill the required columns (flagRow and spectralWindowId).
    // These should ideally be just referenced, but I (SB) could not
    // figure out how to do it.
    //
    Vector<Bool> spwFlagRow=ROMSSpWindowColumns(spectralWindow()).flagRow().getColumn();
    MSDataDescColumns(fakeDDSubTable).flagRow().putColumn(spwFlagRow);

    Vector<Int> spwId(fakeDDSubTable.nrow()); indgen(spwId);
    MSDataDescColumns(fakeDDSubTable).spectralWindowId().putColumn(spwId);
  }
  //
  //----------------------------------------------------------------------------
  //
  const MSDataDescription& CTInterface::dataDescription() 
  {
    return fakeDDSubTable;
  }
  //
  //----------------------------------------------------------------------------
  //
  String CTInterface::columnName(MSMainEnums::PredefinedColumns nameEnum) 
  {
    if (nameEnum == MS::DATA_DESC_ID) 
      return CTEnums::fieldName(CTEnums::SPECTRAL_WINDOW_ID);
    else
      return MS::columnName(nameEnum);
  }
  //----------------------------------------------------------------------------
} //# NAMESPACE CASA - END
