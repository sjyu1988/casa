/*
 * ScantableReader.h
 *
 *  Created on: Jan 5, 2016
 *      Author: nakazato
 */

#ifndef SINGLEDISH_FILLER_SCANTABLE2MSREADER_H_
#define SINGLEDISH_FILLER_SCANTABLE2MSREADER_H_

#include <singledish/Filler/ReaderInterface.h>
#include <singledish/Filler/ScantableIterator.h>

#include <string>
#include <memory>

// casacore includes
#include <casacore/casa/Containers/Record.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class Scantable2MSReader: public ReaderInterface {
public:
  Scantable2MSReader(std::string const &scantable_name);
  virtual ~Scantable2MSReader();

  // get number of rows for MAIN table
  virtual size_t getNumberOfRows() {
    if (!main_table_) {
      return 0;
    }
    return main_table_->nrow();
  }

  virtual Bool isFloatData() const {
    Bool is_float = True;
    if (!main_table_) {
      is_float = False;
    } else {
      String pol_type = main_table_->keywordSet().asString("POLTYPE");
      ROScalarColumn<uInt> polno_column(*main_table_, "POLNO");
      uInt max_pol = max(polno_column.getColumn());
//      std::cout << "pol_type=" << pol_type << " max_pol=" << max_pol << std::endl;
      if ((max_pol == 3) && (pol_type == "linear" || pol_type == "circular")) {
        is_float = False;
      }
    }
//    std::cout << "is_float = " << is_float << std::endl;
    return is_float;
  }

  // to get OBSERVATION table
  virtual Bool getObservationRow(sdfiller::ObservationRecord &record) {
    POST_START;

    Bool return_value = (*this.*get_observation_row_)(record);

    POST_END;
    return return_value;
  }

  // to get ANTENNA table
  virtual Bool getAntennaRow(sdfiller::AntennaRecord &record) {
    POST_START;

    Bool return_value = (*this.*get_antenna_row_)(record);

    POST_END;
    return return_value;
  }

  // to get PROCESSOR table
  virtual Bool getProcessorRow(sdfiller::ProcessorRecord &record) {
    POST_START;

    Bool return_value = (*this.*get_processor_row_)(record);

    POST_END;
    return return_value;
  }

  // to get SOURCE table
  virtual Bool getSourceRow(sdfiller::SourceRecord &record) {
    POST_START;

    Bool return_value = (*this.*get_source_row_)(record);

    POST_END;
    return return_value;
  }

  // to get FIELD table
  virtual Bool getFieldRow(sdfiller::FieldRecord &record) {
    POST_START;

    Bool return_value = (*this.*get_field_row_)(record);

    POST_END;
    return return_value;
  }

  // to get SOURCE table
  virtual Bool getSpectralWindowRow(sdfiller::SpectralWindowRecord &record) {
    POST_START;

    Bool return_value = (*this.*get_spw_row_)(record);

    POST_END;
    return return_value;
  }

  // for DataAccumulator
  virtual Bool getData(size_t irow, sdfiller::DataRecord &record);

protected:
  void initializeSpecific();
  void finalizeSpecific();

private:
  std::unique_ptr<Table> main_table_;
  Table tcal_table_;
  Table weather_table_;

  ROScalarColumn<uInt> scan_column_;ROScalarColumn<uInt> cycle_column_;ROScalarColumn<
      uInt> ifno_column_;ROScalarColumn<uInt> polno_column_;ROScalarColumn<uInt> beam_column_;ROScalarColumn<
      uInt> flagrow_column_;ROScalarColumn<Double> time_column_;ROScalarColumn<
      Double> interval_column_;ROScalarColumn<Int> srctype_column_;
  ArrayColumn<Float> data_column_;
  ArrayColumn<uChar> flag_column_;
  ArrayColumn<Double> direction_column_;
  ArrayColumn<Double> scanrate_column_;ROScalarColumn<String> fieldname_column_;
  ArrayColumn<Float> tsys_column_;ROScalarColumn<uInt> tcal_id_column_;ROScalarColumn<
      uInt> weather_id_column_;
  ArrayColumn<Float> tcal_column_;ROScalarColumn<Float> temperature_column_;ROScalarColumn<
      Float> pressure_column_;ROScalarColumn<Float> humidity_column_;ROScalarColumn<
      Float> wind_speed_column_;ROScalarColumn<Float> wind_direction_column_;
  Vector<uInt> sorted_rows_;
  ScantableFieldIterator::Product field_map_;
  ScantableFrequenciesIterator::Product num_chan_map_;
  std::map<uInt, uInt> tcal_id_map_;
  std::map<uInt, uInt> weather_id_map_;
  String pol_type_;

  Bool (Scantable2MSReader::*get_antenna_row_)(sdfiller::AntennaRecord &);
  Bool (Scantable2MSReader::*get_field_row_)(sdfiller::FieldRecord &);
  Bool (Scantable2MSReader::*get_observation_row_)(
      sdfiller::ObservationRecord &);
  Bool (Scantable2MSReader::*get_processor_row_)(sdfiller::ProcessorRecord &);
  Bool (Scantable2MSReader::*get_source_row_)(sdfiller::SourceRecord &);
  Bool (Scantable2MSReader::*get_spw_row_)(sdfiller::SpectralWindowRecord &);

  std::unique_ptr<ScantableFieldIterator> field_iter_;
  std::unique_ptr<ScantableFrequenciesIterator> freq_iter_;
  std::unique_ptr<ScantableSourceIterator> source_iter_;

  Bool getAntennaRowImpl(sdfiller::AntennaRecord &record);
  Bool getFieldRowImpl(sdfiller::FieldRecord &record);
  Bool getObservationRowImpl(sdfiller::ObservationRecord &record);
  Bool getProcessorRowImpl(sdfiller::ProcessorRecord &record);
  Bool getSourceRowImpl(sdfiller::SourceRecord &record);
  Bool getSpectralWindowRowImpl(sdfiller::SpectralWindowRecord &record);

  template<class _Record>
  Bool noMoreRowImpl(_Record &) {
    POST_START;POST_END;
    return False;
  }

  template<class _Iterator, class _Record, class _Func>
  Bool getRowImplTemplate(std::unique_ptr<_Iterator> &iter, _Record &record,
      _Func &func, typename _Iterator::Product *product = nullptr) {
    POST_START;

    if (!iter) {
      iter.reset(new _Iterator(*main_table_));
    }

    Bool more_data = iter->moreData();
    if (more_data) {
      iter->getEntry(record);
      iter->next();
    } else {
      // seems to be passed through all the table, deallocate iterator
      iter->getProduct(product);
      iter.reset(nullptr);
      // and then redirect function pointer to noMoreRowImpl
      func = &Scantable2MSReader::noMoreRowImpl<_Record>;
    }

    POST_END;

    return more_data;
  }
};

} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_SCANTABLE2MSREADER_H_ */